#define _GNU_SOURCE

#include "observe_child.h"
#include "do_stuff_in_child.h"
#include "cgroup_stuff.h"
#include "global_helpers.h"

#include <unistd.h>
#include <sys/ptrace.h>
#include <sched.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Запускать с sudo
 * Можно попробовать ограничить права с помощью Linux capabilities
*/
int main() {
    int result;

    // Преднастройка parent

    char *cgroup_name = "some-cgroup";

    //TODO: попробовать использовать cgroup namespace, чтобы cgroup удалился после завершения

    if ((result = create_cgroup(cgroup_name)) != 0) {
        return result;
    }

    if ((result = configure_cgroup(cgroup_name)) != 0) {
        return result;
    }

    int pipedes[2];
    if (pipe(pipedes) == -1) {
        return error_out(__LOG);
    }

    // Ниже делаем следующий прикол: форкаем процесс 2 раза
    // Это делаем потому, что вызов unshare() для нового pid неймспейса не перемещает сам процесс в новый pid неймспейс
    // Только форки этого процесса будут в новом pid неймспейсе
    // Чтобы получить pid второго процесса (который станет uut), передаем его pid через pipe

    __pid_t child_pid = fork();

    // Child
    if (child_pid == 0) {
        if (close(pipedes[0]) == -1) {
            return error_out(__LOG);
        }

        return do_stuff_in_child(pipedes[1]);
    }

    printf("Запущен первый child с pid: %d\n", child_pid);

    if (close(pipedes[1]) == -1) {
        return error_out(__LOG);
    }

    pid_t pid_to_observe;

    // Надеемся, что за один раз прочитает из pipe все
    if (read(pipedes[0], &pid_to_observe, sizeof(pid_to_observe)) != sizeof(pid_to_observe)) {
        return error_out(__LOG);
    }

    printf("Получили pid uut: %d\n", pid_to_observe);

    if (close(pipedes[0]) == -1) {
        return error_out(__LOG);
    }

    int status;
    if (waitpid(child_pid, &status, 0) == -1) {
        return error_out(__LOG);
    }
    printf("Оригинальный child завершился со статусом: %d\n", status);

    // Идем наблюдать за uut процессом (здесь он мог еще не сделать exec, это надо учитывать)
    if ((result = observe_child(pid_to_observe)) != 0) {
        return result;
    }

    //TODO: очистить ресурсы: как минимум удалить cgroup

    return 0;
}
