#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main() {
    unsigned int result;

    printf("start\n");

    for (int i = 0; i < 10; i++) {
        printf("#%d Sleep()\n", i);
        result = sleep(i);
        printf("errno: %d\n", errno);
        printf("#%d Sleep result: %d\n", i, result);
    }

    printf("end\n");

    return 0;
}
