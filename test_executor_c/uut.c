#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

void* func(void *ptr) {
    printf("Thread created\n");
    sleep(100);
    return NULL;
}

int main() {
    printf("Hello World\n");

//    pthread_t t;
//    pthread_create(&t, NULL, func, NULL);
//    if (errno != 0) {
//        perror("Error");
//    }

//    FILE *f = fopen("./Makefile", "r");

//    int *arr = calloc(10000, sizeof(int));
//    sleep(20);
//    free(arr);

    sleep(20);
    return 0;
}
