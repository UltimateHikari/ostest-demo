#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

void* func(void *ptr) {
    printf("Thread created\n");
    sleep(100);
    return NULL;
}

int main() {
    printf("Hello World\n");

    sleep(10);

    printf("Started\n");

    int pipedes[2];
    if (pipe(pipedes) == -1) {
        perror("pipe() failed");
        return 1;
    }

    __pid_t pid = fork();

    if (pid == -1) {
        perror("fork() failed");
        return 1;
    }

    if (pid == 0) {
        // child

        close(pipedes[1]);

        #define BUF_SIZE 255
        char buf[BUF_SIZE] = {0};
        ssize_t read_idx = 0;
        ssize_t n_read;
        while ((n_read = read(pipedes[0], buf + read_idx, 5)) > 0) {
            printf("Read %ld bytes\n", n_read);
            if (read_idx + n_read > BUF_SIZE) {
                printf("buffer size %d is not enough to store the read message\n", BUF_SIZE);
                return 1;
            }
            read_idx += n_read;
        }
        #undef BUF_SIZE

        if (n_read == -1) {
            perror("read() failed");
            return 1;
        }

        printf("Received %ld bytes in child: %s\n", read_idx, buf);

        close(pipedes[0]);

        return 0;
    }

    close(pipedes[0]);

    char *buf = "12345678901234567890123";
    size_t to_write = strlen(buf) + 1;
    size_t write_idx = 0;
    size_t n_written;
    while ((n_written = write(pipedes[1], buf + write_idx, to_write - write_idx)) > 0) {
        printf("Written %ld bytes\n", n_written);
        write_idx += n_written;
        if (write_idx == to_write) {
            break;
        }
    }
    if (n_written == 0) {
        printf("something happened - write() returned 0\n");
        return 1;
    }
    if (n_written == -1) {
        perror("write() failed");
        return 1;
    }

    close(pipedes[1]);

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid() failed");
        return 1;
    }

    printf("child ended with status %d\n", status);

//    pthread_t t;
//    pthread_create(&t, NULL, func, NULL);
//    if (errno != 0) {
//        perror("Error");
//    }

//    FILE *f = fopen("./Makefile", "r");

//    int *arr = calloc(10000, sizeof(int));
//    sleep(20);
//    free(arr);

    return 0;
}
