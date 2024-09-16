#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
#include <time.h>

int main() {
    unsigned int result;

    printf("start\n");

    for (int i = 0; i < 5; i++) {
        printf("#%d Sleep()\n", i);
        result = sleep(i);
        printf("errno: %d\n", errno);
        printf("#%d Sleep result: %d\n", i, result);
    }

    for (int i = 0; i < 5; i++) {
        printf("#%d Syscall clock_nanosleep()\n", i);
        struct timespec request = {.tv_sec = 1, .tv_nsec = 0};
        struct timespec remain = {.tv_sec = 0, .tv_nsec = 0};
        result = syscall(SYS_clock_nanosleep, CLOCK_REALTIME, 0, &request, &remain);
        printf("errno: %d\n", errno);
        printf("#%d Syscall clock_nanosleep result: %d\n", i, result);
    }

    printf("end\n");

    return 0;
}
