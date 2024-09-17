#define _GNU_SOURCE

#include "global_helpers.h"
#include "namespace_stuff.h"

#include <sched.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int enter_required_namespaces() {
    // Заходим в новый mount namespace
    // Все еще все маунты хоста остаются доступными
    if (unshare(CLONE_NEWNS) == -1) {
        return error_out(__LOG);
    }

    // Если не сделать это, то mount'ы ниже будут видны на хосте (т.к. по умолчанию маунты - SHARED)
    if (mount(NULL, "/", NULL, MS_PRIVATE | MS_REC, NULL) == -1) {
        return error_out(__LOG);
    }

    if (mount("uut-tmpfs", "./uut-folder/tmpfs", "tmpfs", 0, "size=1M") == -1) {
        return error_out(__LOG);
    }

    if (mount("uut-lower2-tmpfs", "./uut-folder/lower2", "tmpfs", 0, "size=50M") == -1) {
        return error_out(__LOG);
    }

    int result = system("mkdir ./uut-folder/tmpfs/work && mkdir ./uut-folder/tmpfs/upper && cp ./uut ./uut-folder/lower2");
    if (result == -1) {
        return error_out(__LOG);
    }
    if (result != 0) {
        printf(__LOG ": system() failed with return code %d\n", result);
        return result;
    }

    // Не уверен, куда маунтить надо системные маунты (lower/upper/merged)
    // +Потенциально можно обойти лимит диска tmpfs, если писать в эти маунты

    if (mount("uut-overlay", "./uut-folder/merged", "overlay", 0, "lowerdir=./uut-folder/lower:./uut-folder/lower2,upperdir=./uut-folder/tmpfs/upper,workdir=./uut-folder/tmpfs/work") == -1) {
        return error_out(__LOG);
    }

    // Обычно после chroot мы теряем доступ ко всем файлам кроме тех, который лежат в uut-folder (то есть нельзя запустить /bin/bash, нельзя использовать в Си system(), прочее)
    // Но мы ранее замаунтили что-то в uut-folder, так что часть проблем решена
    if (chroot("./uut-folder/merged") == -1) {
        return error_out(__LOG);
    }

    // chroot не меняет текущую рабочую директорию, это должны сделать мы
    if (chdir("/") == -1) {
        return error_out(__LOG);
    }

    return 0;
}

int unshare_pid() {
    if (unshare(CLONE_NEWPID) == -1) {
        return error_out(__LOG);
    }

    return 0;
}

int mount_proc_sys_dev() {
    if (mount("proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_RELATIME, NULL) == -1) {
        return error_out(__LOG);
    }

    if (mount("sys", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_RELATIME, NULL) == -1) {
        return error_out(__LOG);
    }

    if (mount("dev", "/dev", "devtmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_RELATIME, NULL) == -1) {
        return error_out(__LOG);
    }

    return 0;
}
