#include "stdio.h"
#include "stdlib.h"

int main() {
	int s = 0;
	for (int i = 0; i < 1000*1000*1000; ++i) {
		s += 1;
	}
	printf("end %d\n", s);
	return 0;
}
