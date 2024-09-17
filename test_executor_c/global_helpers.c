#define _GNU_SOURCE

#include "global_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

int error_out(char* log) {
    int saved_errno = errno;
    perror(log);
    return saved_errno;
}

void generate_random_string(char *buf, int size) {
    srand(time(NULL));

    char alphabet[] = "0123456789abcdef";

    for (int i = 0; i < size; i++) {
        int index = (int) ((double) rand() / RAND_MAX * (sizeof(alphabet) - 1));
        buf[i] = alphabet[index];
    }
}
