#pragma once

/**
 * В child процессе применяем настройки (cgroup, прочее) и меняем этот child процесс на запускаемый uut (unit under test).
 * Uut будет запущен с необходимыми лимитами. Parent процесс увидит pid child'а и сможет за чем-то следить.
 */
int do_stuff_in_child();
