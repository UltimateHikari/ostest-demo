#pragma once

#define S1(x) #x
#define S2(x) S1(x)
#define __LOG "Error: " __FILE__ ": " S2(__LINE__)

#define STRING_FORMAT(name, pattern, ...) char name[255] = {0}; \
{\
    int n_written = snprintf(name, 255, pattern, __VA_ARGS__);\
    if (n_written < 0 || n_written >= 255) {\
        printf(__LOG ": snprintf error, result: %d\n", n_written);\
        return n_written;\
    }\
}

int error_out(char* log);
void generate_random_string(char *buf, int size);
