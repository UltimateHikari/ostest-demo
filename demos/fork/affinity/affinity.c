#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    const char* s = getenv("AFFINITY_OPTION");
    char* args[argc+4];
    char str[256];
    strcpy(str, "./aff/");
    strcat(str, argv[0]);
    args[0] = "taskset";
    args[1] = "-c";
    args[2] = s;
    args[3] = str;
    int i=0;

    for (int i = 1; i < argc; ++i) {
        args[i+3] = argv[i];
    }
    args[argc+3] = NULL;
    if (execvp("taskset", args) == -1) {
        perror("execvp error");
        return -1;
    }
}
