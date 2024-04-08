cmake_minimum_required(VERSION 3.22)
project(ostest C)

set(CMAKE_C_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(ostest main.c observe_child.c observe_child.h global_helpers.c global_helpers.h do_stuff_in_child.c do_stuff_in_child.c do_stuff_in_child.h cgroup_stuff.c cgroup_stuff.h mount_namespace_stuff.c mount_namespace_stuff.h strace_observe_stuff.c strace_observe_stuff.h)

add_executable(uut uut/uut.c)
target_link_libraries(uut PRIVATE Threads::Threads)
