#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

#define S1(x) #x
#define S2(x) S1(x)
#define __LOG "Error: " __FILE__ ": " S2(__LINE__) " "

int main() {
    // Замена stdin на новый
    int pipedes[2] = {0};
    if (pipe(pipedes) == -1) {
        perror(__LOG "pipe() failed");
        return 1;
    }
    if (dup2(pipedes[0], STDIN_FILENO) == -1) {
        perror(__LOG "dup2() failed");
        return 1;
    }
    if (close(pipedes[1]) == -1) {
        perror(__LOG "close() failed");
        return 1;
    }

    // Замена stdout на новый
    int new_stdout = open("./stdout.txt", O_WRONLY | O_CREAT | O_TRUNC);
    if (new_stdout == -1) {
        perror(__LOG "open() failed");
        return 1;
    }
    if (dup2(new_stdout, STDOUT_FILENO) == -1) {
        perror(__LOG "dup2() failed");
        return 1;
    }

    // Замена stderr на новый
    int new_stderr = open("./stderr.txt", O_WRONLY | O_CREAT | O_TRUNC);
    if (new_stderr == -1) {
        perror(__LOG "open() failed");
        return 1;
    }
    if (dup2(new_stderr, STDERR_FILENO) == -1) {
        perror(__LOG "dup2() failed");
        return 1;
    }

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
