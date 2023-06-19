#include <stdio.h>
#include <unistd.h>

int main() {
	long pageSize = sysconf(_SC_PAGESIZE);
	printf("La dimensione della pagina di memoria è: %ld bytes\n", pageSize);
	return 0;
}
