#define _GNU_SOURCE

#include "do_stuff_in_child.h"
#include "global_helpers.h"
#include "cgroup_stuff.h"
#include "namespace_stuff.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <sys/ptrace.h>
#include <time.h>
#include <sys/stat.h>

/**
 * Делаем второй fork и меняем новый child (запущенный через новый fork) на uut через exec*
 */
int run_uut(int pipedes);

int do_stuff_in_child(int pipedes) {
    printf("Начало настройки ограничений для uut\n");

    int result;

    // Включаем cgroup для процесса
    if ((result = apply_cgroup_to_process()) != 0) {
        return result;
    }

    // Заводим процесс в нужные неймспейсы и прочие ограничения
    if ((result = enter_required_namespaces()) != 0) {
        return result;
    }

    printf("Настройка готова, запускаем uut через еще один fork (и подключение strace)\n");

    return run_uut(pipedes);
}

int run_uut(int pipedes) {
    // Переходим в pid namespace - но этот процесс НЕ переходит в новый pid namespace, пока мы не сделаем второй форк
    // Внимание: нельзя создавать child'ы процесса между unshare pid и fork, иначе pid namespace будет уничтожен (или около того) после завершения child'а
    // Например, нельзя вызывать system() в Си
    if (unshare(CLONE_NEWPID) == -1) {
        return error_out(__LOG);
    }

    // Отказываемся от sudo прав перед запуском uut
    if (setuid(9999) == -1) {
        return error_out(__LOG);
    }

    pid_t pid_of_a_new_child = fork();

    if (pid_of_a_new_child == -1) {
        return error_out(__LOG);
    }

    // Child of a child
    if (pid_of_a_new_child == 0) {
        // Обязательно закрываем пайп в uut, чтобы следа от этого не оставалось
        if (close(pipedes) == -1) {
            return error_out(__LOG);
        }

        //TODO: найти более хороший способ ждать strace. Сейчас надеемся на то, что за время sleep он успел подключиться.
        struct timespec time_to_sleep = {.tv_sec=2, .tv_nsec = 0};
        clock_nanosleep(CLOCK_REALTIME, 0, &time_to_sleep, NULL);

        char *argv[] = {"uut", 0};
        if (execvp("./uut", argv) == -1) {
            return error_out(__LOG);
        }
        return 0;
    }

    // Надеемся, что за один раз запишет в pipe все
    if (write(pipedes, &pid_of_a_new_child, sizeof(pid_of_a_new_child)) != sizeof(pid_of_a_new_child)) {
        return error_out(__LOG);
    }

    if (close(pipedes) == -1) {
        return error_out(__LOG);
    }

    return 0;
}
