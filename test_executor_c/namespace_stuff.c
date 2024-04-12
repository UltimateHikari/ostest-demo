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

    if (mount("uut-mount", "./uut-folder", "tmpfs", 0, "size=1M") == -1) {
        return error_out(__LOG);
    }

    int result = system("mkdir ./uut-folder/proc && mkdir ./uut-folder/sys && cp ./uut ./uut-folder");
    if (result == -1) {
        return error_out(__LOG);
    }
    if (result != 0) {
        printf(__LOG ": system() failed with return code %d\n", result);
        return result;
    }

    // Точно маунтим proc, чтобы нельзя было видить процессы хоста
    if (mount("new_proc", "./uut-folder/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_RELATIME, NULL) == -1) {
        return error_out(__LOG);
    }

    if (mount("new_sysfs", "./uut-folder/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_RELATIME, NULL) == -1) {
        return error_out(__LOG);
    }

    // Обычно после chroot мы теряем доступ ко всем файлам кроме тех, который лежат в uut-folder (то есть нельзя запустить /bin/bash, нельзя использовать в Си system(), прочее)
    // Но мы ранее замаунтили что-то в uut-folder, так что часть проблем решена
    //TODO: если делать chroot, то дальше фейлится exec (и все равно нельзя запустить bash, т.к. не замаунтили его), попробовать использовать overlayfs
//    if (chroot("./uut-folder") == -1) {
//        return error_out(__LOG);
//    }

    // chroot не меняет текущую рабочую директорию, это должны сделать мы
//    if (chdir("/") == -1) {
//        return error_out(__LOG);
//    }

    return 0;
}
