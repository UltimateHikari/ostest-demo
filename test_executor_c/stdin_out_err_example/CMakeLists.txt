cmake_minimum_required(VERSION 3.22)
project(strace_tampering_example C)

set(CMAKE_C_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(uut_runner uut_runner.c)

add_executable(uut uut.c)
target_link_libraries(uut PRIVATE Threads::Threads)
