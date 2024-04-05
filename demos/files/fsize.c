#include <stdio.h>
  
int main(void)
{
    FILE * fd = fopen("large_file", "w");
    for (int i = 0; i < 1000000; ++i) {
        putc('a', fd);
    }
    fclose(fd);
    return 0;
}
