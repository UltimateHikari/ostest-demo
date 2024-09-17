#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/user.h>
#include <stdlib.h>

#define S1(x) #x
#define S2(x) S1(x)
#define __LOG "Error: " __FILE__ ": " S2(__LINE__) " "

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Needs single integer argument - pid of a process to ptrace\n");
        return 1;
    }
    char *p;
    int pid = strtol(argv[1], &p, 10);
    if (*p != '\0') {
        printf("strtol() failed on argument: %s\n", argv[1]);
        return 1;
    }

    printf("pid: %d\n", pid);

    if (ptrace(PTRACE_SEIZE, pid, 0, 0) == -1) { // Забрать процесс
        perror(__LOG "ptrace() failed");
        return 1;
    }

    // Важно: после того, как забрали процесс, можем использовать waitpid на этот процесс, даже если он не наш child
    // А вообще waitpid можно использовать только на свой child

    int status;

    if (waitpid(pid, &status, WUNTRACED) == -1) { // Ждем первоначального SIGSTOP (процесс запустился в остановленном состоянии с помощью SIGSTOP)
        perror(__LOG "waitpid() failed");
        return 1;
    }
    if (!WIFSTOPPED(status)) {
        printf(__LOG "Initial status is not stopped\n");
        return 1;
    }
    if (WSTOPSIG(status) != SIGSTOP) {
        printf(__LOG "Initial status is not SIGSTOP\n");
        return 1;
    }

    if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) { // Продолжить процесс и остановить его сигналом при входе в следующий системный вызов (и выходе из системного вызова)
        perror(__LOG "ptrace() failed");
        return 1;
    }

    while (true) {
        if (waitpid(pid, &status, 0) == -1) { // Ждем пока не придет сигнал / процесс не завершится
            perror(__LOG "waitpid() failed");
            return 1;
        }

        if (!WIFSTOPPED(status)) {
            printf(__LOG "Status is not stopped\n");
            return 0;
        }

        struct user_regs_struct uregs;
        if (ptrace(PTRACE_GETREGS, pid, 0, &uregs) == -1) {
            perror(__LOG "ptrace() failed");
            return 1;
        }
        printf("orig_rax (syscall number): %lld\n", uregs.orig_rax);
        if (uregs.orig_rax == 230) { // clock_nanosleep
            uregs.orig_rax = -1;
            if (ptrace(PTRACE_SETREGS, pid, 0, &uregs) == -1) { // Замена номера системного вызова -1 (через регистр)
                perror(__LOG "ptrace() failed");
                return 1;
            }

            // Другой способ замены номера системного вызова на -1
//            if (ptrace(PTRACE_POKEUSER, pid, 8 * 15, -1) == -1) { // 15 = ORIG_RAX
//                perror(__LOG "ptrace() failed");
//                return 1;
//            }
        }

        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) {
            perror(__LOG "ptrace() failed");
            return 1;
        }
    }

    return 0;
}
