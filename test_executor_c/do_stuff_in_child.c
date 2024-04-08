#define _GNU_SOURCE

#include "do_stuff_in_child.h"
#include "global_helpers.h"
#include "cgroup_stuff.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <sys/ptrace.h>
#include <time.h>

/**
 * Меняем текущий процесс на тестируемую прогу.
 * Т.к. запускаем в child процессе, то по сути меняем child процесс на тестируемую прогу.
 */
int run_uut();

int do_stuff_in_child() {
    __pid_t pid = getpid();
    if (pid == -1) {
        return error_out(__LOG);
    }

    printf("Начало настройки ограничений для uut, pid: %d\n", pid);

    int result;

//    if ((result = enter_mount_namespace()) != 0) {
//        return result;
//    }
//
//    if ((result = create_restricted_mount() != 0)) {
//        return result;
//    }

    // Включаем cgroup для процесса
    if ((result = apply_cgroup_to_process()) != 0) {
        return result;
    }

    // После chroot нельзя запустить ничего, кроме того, что лежит в uut-folder.
    // То есть нельзя запустить /bin/bash, нельзя использовать в Си system("pwd"), прочее
//    if (chroot("./uut-folder") == -1) {
//        return error_out(__LOG);
//    }

    // Отказываемся от sudo прав перед запуском uut
    if (setuid(9999) == -1) {
        return error_out(__LOG);
    }

    printf("Настройка готова, запускаем uut (после подключения strace)\n");

    // Здесь происходит прикол. Sleep мог бы ждать вечно.
    // Но через подключение strace мы заставляем sleep зафейлиться и программу продолжить выполнение.
    struct timespec time_to_sleep = {.tv_sec=999999999, .tv_nsec = 0};
    clock_nanosleep(CLOCK_REALTIME, 0, &time_to_sleep, NULL);

    return run_uut();

//    char *argv[] = {"exec123", 0};
//    if (execvp("/bin/bash", argv) == -1) {
//        return error_out(__LOG);
//    }
}

int run_uut() {
    char *argv[] = {"uut", 0};
    if (execvp("./uut", argv) == -1) {
        return error_out(__LOG);
    }
    return 0;
}
