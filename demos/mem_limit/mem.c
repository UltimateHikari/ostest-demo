#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    printf("start\n");
    int* s = malloc(1000000);
    s[0] = 0;
    free(s);
    printf("end\n");
}
