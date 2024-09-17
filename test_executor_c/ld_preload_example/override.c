#include <unistd.h>
#include <stdio.h>

unsigned int sleep(unsigned int seconds) {
    printf("sleep(%d)\n", seconds);
    return 5;
}

long syscall(long number, ...) {
    printf("syscall(%ld, ...)\n", number);
    return 10;
}
