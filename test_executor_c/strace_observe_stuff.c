#define _GNU_SOURCE

#include "global_helpers.h"
#include "strace_observe_stuff.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <string.h>

int start_ptrace_and_write_to_file(__pid_t pid, char *strace_command, char *strace_output_file_name);

int observe_with_strace(__pid_t pid) {
    int result;

    // Трейсим через strace
    // К сожалению, в strace попадает куча вызовов до exec* - их фильтруем
    // Было бы хорошо вызывать strace как библиотеку в Си, но такого готового нет

    //TODO: Сейчас надеемся на то, что мы успеем подключиться за время sleep к child
    // Найти более хороший способ заставить child ждать подключения strace

    //TODO: можно тесты на tampering (какие-то системные вызовы делают ошибки - посмотреть поведение программы в таком случае)

    char random_str[16] = {0};
    generate_random_string(random_str, 10);
    STRING_FORMAT(strace_output_file_name, "./strace_output/%s.txt", random_str)

    STRING_FORMAT(strace_command, "strace -f -p %d 2>&1", pid)

    if ((result = start_ptrace_and_write_to_file(pid, strace_command, strace_output_file_name)) != 0) {
        return result;
    }

    //TODO: сюда тесты на проверку вывода strace
}

int start_ptrace_and_write_to_file(__pid_t pid, char *strace_command, char *strace_output_file_name) {
    FILE *strace_f = popen(strace_command, "r");
    if (strace_f == NULL) {
        return error_out(__LOG);
    }

    FILE *strace_output_log = fopen(strace_output_file_name, "w");
    if (strace_output_log == NULL) {
        return error_out(__LOG);
    }

    char buf[1024] = {0};
    while (fgets(buf, 1024, strace_f)) {
        //TODO: error handling
        fwrite(buf, sizeof(char), strlen(buf) + 1, strace_output_log);
    }

    if (pclose(strace_f) == -1) {
        return error_out(__LOG);
    }

    if (fclose(strace_output_log) != 0) {
        return error_out(__LOG);
    }

    return 0;
}
