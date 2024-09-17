#define _GNU_SOURCE

#include "global_helpers.h"
#include "observe_child.h"
#include "strace_observe_stuff.h"

#include <stdio.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <errno.h>

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

    struct stat stat_res;
    char stat_file[64] = {0};
    sprintf(stat_file, "/proc/%d", pid);
    while ((result = stat(stat_file, &stat_res)) != -1) {}
    if (errno != ENOENT) {
        return error_out(__LOG);
    }

    printf("UUT успешно завершился\n");
}
