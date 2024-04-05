#include <stdio.h>
  
int main(void)
{
    FILE * ffs[20];
    for (int i = 0; i < 20; ++i) {
        char str[10];
        sprintf(str, "test%d", i);
        ffs[i] = fopen(str, "w");
        if (ffs[i] == NULL) {
            perror(str);
            return 1;
        }
    }
    for (int i = 0; i < 20; ++i) {
        fclose(ffs[i]);
    }
    return 0;
}
