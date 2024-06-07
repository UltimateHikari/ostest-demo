#pragma once

/*
 *  Создаем cgroup
 *  Возможно, не все контроллеры будут активны. У меня все контроллеры активны по-дефолту.
 *  Сюда нужно название cgroup именно для этого процесса (а сейчас стоит некий some-cgroup)
 *  Если несколько процессов шарят одну cgroup, то они едят ресурсы из одного пула (на который лимит стоит)
 */
int create_cgroup(char *cgroup_name);

/**
 * Добавление ограничений в созданную cgroup
 */
int configure_cgroup(char *cgroup_name);

int write_cgroup_value(char *cgroup_name, char *controller_type, char *value_to_write);

/**
 * Ставим cgroup процесса
 */
int apply_cgroup_to_process();
