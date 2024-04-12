#define _GNU_SOURCE

#include "global_helpers.h"
#include "cgroup_stuff.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int create_cgroup(char *cgroup_name) {
    STRING_FORMAT(command, "rmdir /sys/fs/cgroup/%s; mkdir -p /sys/fs/cgroup/%s; true", cgroup_name, cgroup_name)

    int result;
    if ((result = system(command)) != 0) {
        printf(__LOG ": system(\"%s\") error, result: %d\n", command, result);
        return result;
    }
    printf("cgroup created\n");
    return 0;
}

int configure_cgroup(char *cgroup_name) {
    int result;

    // Пример: макс оперативная память - киляет процесс
    if ((result = write_cgroup_value(cgroup_name, "memory.max", "10000000")) != 0) {
        return result;
    }

    // Пример: макс число tid - не киляет процесс, возвращает Resource temporary unavailable при попытке создания нового потока
    if ((result = write_cgroup_value(cgroup_name, "pids.max", "100")) != 0) {
        return result;
    }

    printf("cgroup configured\n");

    return 0;
}

int write_cgroup_value(char *cgroup_name, char *controller_type, char *value_to_write) {
    STRING_FORMAT(cgroup_file, "/sys/fs/cgroup/%s/%s", cgroup_name, controller_type)

    int fd = open(cgroup_file, O_WRONLY);
    if (fd == -1) {
        return error_out(__LOG);
    }
    if (write(fd, value_to_write, strlen(value_to_write) + 1) == -1) {
        return error_out(__LOG);
    }
    close(fd);
    return 0;
}

int apply_cgroup_to_process() {
    __pid_t pid = getpid();

    // Запись pid в этот файл меняет cgroup процесса pid
    int fd = open("/sys/fs/cgroup/some-cgroup/cgroup.procs", O_WRONLY);
    if (fd == -1) {
        return error_out(__LOG);
    }
    // Без превращения в строку не работает
    char pid_str[16] = {0};
    sprintf(pid_str, "%d", pid);
    if (write(fd, &pid_str, strlen(pid_str) + 1) == -1) {
        return error_out(__LOG);
    }
    // Обязательно закрываем, иначе не проставится cgroup
    close(fd);
    return 0;
}
