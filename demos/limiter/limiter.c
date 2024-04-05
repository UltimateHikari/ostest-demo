#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Invalid argumet number: expected at least 4");
        return -1;
    }
    if (strcmp(argv[1], "mem") != 0 &&
        strcmp(argv[1], "data_mem") != 0 &&
        strcmp(argv[1], "fsize") != 0 &&
        strcmp(argv[1], "fd") != 0 &&
        strcmp(argv[1], "child") != 0) {
        fprintf(stderr, "Invalid 1 argumet. Expected mem, data_mem, fd, fsize"
                        ", child. Got %s\n", argv[1]);
        return -1;
    }
    int resource = 0;
    if (strcmp(argv[1], "mem") == 0) {
        resource = RLIMIT_AS;
    }
    if (strcmp(argv[1], "data_mem") == 0) {
        resource = RLIMIT_DATA;
    }
    if (strcmp(argv[1], "fsize") == 0) {
        resource = RLIMIT_FSIZE;
    }
    if (strcmp(argv[1], "fd") == 0) {
        resource = RLIMIT_NOFILE;
    }
    if (strcmp(argv[1], "child") == 0) {
        resource = RLIMIT_NPROC;
    }

    struct rlimit limit;
    limit.rlim_cur = atoi(argv[2]);
    limit.rlim_max = atoi(argv[2]);

    if (setrlimit(resource, &limit) == -1) {
        perror("setlimit error");
        return -1;
    };
    if (execvp(argv[3], &argv[3]) == -1) {
        perror("execvp error");
        return -1;
    }
}
