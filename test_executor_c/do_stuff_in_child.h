#pragma once

/**
 * В child процессе применяем настройки (cgroup, прочее),
 * делаем второй форк,
 * передаем pid нового child'а через pipedes
 * и меняем второй child процесс через exec* на запускаемый uut (unit under test).
 */
int do_stuff_in_child(int pipedes);
