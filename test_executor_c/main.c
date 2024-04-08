#define _GNU_SOURCE

#include "observe_child.h"
#include "do_stuff_in_child.h"
#include "cgroup_stuff.h"

#include <unistd.h>
#include <sys/ptrace.h>

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

    // Можно было бы использовать vfork, чтобы нужно заставить parent процесс ждать, пока не произойдет exec* в child
    // Но используем обычный fork, т.к. мы трейсим через strace и разрешаем сделать exec* в child только после подключения strace.
    __pid_t pid = fork();

    if (pid == 0) {
        return do_stuff_in_child();
    }

    if ((result = observe_child(pid)) != 0) {
        return result;
    }

    //TODO: очистить ресурсы: как минимум удалить cgroup

    return 0;
}
