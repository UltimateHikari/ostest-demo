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

    // И тут возникает прикол: uut не наш child (мы сделали двойной fork())
    // Нельзя ждать его завершения через wait() / waitpid()
    //TODO: сделать ожидание завершения процесса каким-то способом. Пока что просто делаем sleep...
    sleep(10000000);

    //printf("Uut завершился со статусом %d\n", status);
}
