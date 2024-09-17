#include <unistd.h>
#include <stdio.h>
#include <signal.h>

int main() {
    // Ждем пока основной процесс не продолжит наше исполнение
    if (raise(SIGSTOP) == -1) {
        perror("raise() failed");
        return 1;
    }

    // Меняем себя на uut
    char *argv[] = {"uut", 0};
    if (execvp("./uut", argv) == -1) {
        perror("execvp() failed");
        return 1;
    }

    return 0;
}
