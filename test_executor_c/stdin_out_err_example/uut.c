#include <stdio.h>
#include <unistd.h>

int main() {
    printf("start\n");

    for (int i = 0; i < 4; i++) {
        int number;
        scanf("%d", &number);
        if (i % 2 == 0) {
            printf("stdout: %d\n", number);
        } else {
            fprintf(stderr, "stderr: %d\n", number);
        }
        sleep(1);
    }

    printf("end\n");

    return 0;
}
