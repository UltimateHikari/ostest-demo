#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    FILE *file = fopen("/home/123.txt", "w");

    if (file == NULL) {
        perror("fopen() failed");
        return 1;
    }

    int num_written = 0;

    for (int i = 0; i < 1000000; i++) {
        char *str = "123456789\n";

        int result = fwrite(str, sizeof(char), strlen(str), file);
        num_written += result;

        if (result < strlen(str)) {
            perror("fwrite() failed");
            printf("num_written = %d\n", num_written);
            return 1;
        }
    }


    if (fclose(file) == EOF) {
        perror("fclose() failed");
        return 1;
    }

    return 0;
}
