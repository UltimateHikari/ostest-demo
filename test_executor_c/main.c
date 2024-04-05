#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sched.h>
#include <sys/mount.h>

#define S1(x) #x
#define S2(x) S1(x)
#define __LOG "Error: " __FILE__ ": " S2(__LINE__)

int error_out();
int do_stuff_in_child();
int configure_cgroup(char *cgroup_name);
int apply_cgroup_to_process();
int write_cgroup_value(char *cgroup_name, char *controller_type, char *value_to_write);
int create_cgroup(char *cgroup_name);
int run_uut();
int enter_mount_namespace();
int create_restricted_mount();

/*
 * Запускать с sudo
 * Можно попробовать ограничить права с помощью Linux capabilities
*/
int main() {
    int result;

    char *cgroup_name = "some-cgroup";

    if ((result = create_cgroup(cgroup_name)) != 0) {
        return result;
    }

    if ((result = configure_cgroup(cgroup_name)) != 0) {
        return result;
    }

    // vfork, потому что нужно заставить parent процесс ждать, пока не произойдет exec* в child
    __pid_t pid = vfork();

    if (pid == 0) {
        return do_stuff_in_child();
    }

    //TODO: Следим за uut здесь

    // Ждем завершения uut
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        return error_out(__LOG);
    }

    printf("Uut завершился со статусом %d\n", status);

    //TODO: очистить ресурсы: как минимум удалить cgroup

    return 0;
}

int create_restricted_mount() {
    int result;

    if ((result = system("pwd")) != 0) {
        printf(__LOG ": system() error, result: %d\n", result);
        return result;
    }

    if ((result = system("mount -t tmpfs -o size=1M uut-mount ./uut-folder")) != 0) {
        printf(__LOG ": system() error, result: %d\n", result);
        return result;
    }

    if ((result = system("cp uut ./uut-folder")) != 0) {
        printf(__LOG ": system() error, result: %d\n", result);
        return result;
    }

    if ((result = system("pwd")) != 0) {
        printf(__LOG ": system() error, result: %d\n", result);
        return result;
    }

    if (chroot("./uut-folder") == -1) {
        return error_out(__LOG);
    }

    return 0;
}

/*
 *  Создаем cgroup
 *  Возможно, не все контроллеры будут активны. У меня все контроллеры активны по-дефолту.
 *  Сюда нужно название cgroup именно для этого процесса (а сейчас стоит некий some-cgroup)
 *  Если несколько процессов шарят одну cgroup, то они едят ресурсы из одного пула (на который лимит стоит)
 */
int create_cgroup(char *cgroup_name) {
    #define BUF_SIZE 255
    char command[BUF_SIZE] = {0};
    int n_written = snprintf(command, BUF_SIZE, "rmdir /sys/fs/cgroup/%s; mkdir -p /sys/fs/cgroup/%s; true", cgroup_name, cgroup_name);
    if (n_written < 0 || n_written >= BUF_SIZE) {
        printf(__LOG ": snprintf error, result: %d\n", n_written);
        return n_written;
    }
    #undef BUF_SIZE

    int result;
    if ((result = system(command)) != 0) {
        printf(__LOG ": system() error, result: %d\n", result);
        return result;
    }
    printf("cgroup created\n");
    return 0;
}

/**
 * В child процессе применяем настройки (cgroup, прочее) и меняем этот child процесс на запускаемый uut (unit under test).
 * Uut будет запущен с необходимыми лимитами. Parent процесс увидит pid child'а и сможет за чем-то следить.
 */
int do_stuff_in_child() {
    printf("Начало настройки ограничений для uut, pid: %d\n", getpid());

    int result;

//    if ((result = enter_mount_namespace()) != 0) {
//        return result;
//    }

    if ((result = create_restricted_mount() != 0)) {
        return result;
    }

    // Включаем cgroup для процесса
    if ((result = apply_cgroup_to_process()) != 0) {
        return result;
    }

    // Отказываемся от sudo прав перед запуском uut
    if (setuid(9999) == -1) {
        return error_out(__LOG);
    }

    printf("Настройка готова, запускаем uut\n");

    //return run_uut();

    if ((result = system("")) != 0) {
        printf(__LOG ": system() error, result: %d\n", result);
        return result;
    }
}

/**
 * Заходим в mount namespace. В любой другой можно так же.
 */
int enter_mount_namespace() {
    __pid_t pid = getpid();

    #define BUF_SIZE 255
    char mount_namespace_file[BUF_SIZE] = {0};
    int n_written = snprintf(mount_namespace_file, BUF_SIZE, "/proc/%d/ns/mnt", pid);
    if (n_written < 0 || n_written >= BUF_SIZE) {
        printf(__LOG ": snprintf error, result: %d\n", n_written);
        return n_written;
    }
    #undef BUF_SIZE

    int fd = open(mount_namespace_file, O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        return error_out(__LOG);
    }

    // Заходим в mount namespace
    if (setns(fd, CLONE_NEWNS) == -1) {
        return error_out(__LOG);
    }

    return 0;
}

/**
 * Добавление ограничений в созданную cgroup
 */
int configure_cgroup(char *cgroup_name) {
    int result;

    // Пример: макс оперативная память - киляет процесс
    if ((result = write_cgroup_value(cgroup_name, "memory.max", "2000000")) != 0) {
        return result;
    }

    // Пример: макс число tid - не киляет процесс, возвращает Resource temporary unavailable при попытке создания нового потока
    if ((result = write_cgroup_value(cgroup_name, "pids.max", "10")) != 0) {
        return result;
    }

    printf("cgroup configured\n");

    return 0;
}

int write_cgroup_value(char *cgroup_name, char *controller_type, char *value_to_write) {
    #define BUF_SIZE 255
    char cgroup_file[BUF_SIZE] = {0};
    int n_written = snprintf(cgroup_file, BUF_SIZE, "/sys/fs/cgroup/%s/%s", cgroup_name, controller_type);
    if (n_written < 0 || n_written >= BUF_SIZE) {
        printf(__LOG ": snprintf error, result: %d\n", n_written);
        return n_written;
    }
    #undef BUF_SIZE

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

/**
 * Ставим cgroup процесса
 */
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

/**
 * Меняем текущий процесс на тестируемую прогу.
 * Т.к. запускаем в child процессе, то по сути меняем child процесс на тестируемую прогу.
 */
int run_uut() {
    char *argv[] = {"uut", 0};
    if (execvp("./uut", argv) == -1) {
        return error_out(__LOG);
    }
    return 0;
}

int error_out(char* log) {
    int saved_errno = errno;
    perror(log);
    return saved_errno;
}
