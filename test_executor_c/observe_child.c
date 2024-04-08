#define _GNU_SOURCE

#include "global_helpers.h"
#include "observe_child.h"
#include "strace_observe_stuff.h"

#include <stdio.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

int observe_child(__pid_t pid) {
    int result;

    // Можно попробовать запустить несколько потоков и наблюдать с помощью разных инструментов
    // Пока что наблюдаем одним за раз

    // Наблюдаем с помощью strace
    if ((result = observe_with_strace(pid)) != 0) {
        return result;
    }

    //TODO: наблюдаем смотря на procfs

    // Ждем завершения uut
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        return error_out(__LOG);
    }

    printf("Uut завершился со статусом %d\n", status);
}
