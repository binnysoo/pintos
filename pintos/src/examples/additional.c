#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>

int
main (int argc, char **argv) {
	if (argc < 5)
		return EXIT_FAILURE;
	int fib = fibonacci(atoi(argv[1]));
	int max = max_of_four_int(atoi(argv[1]), atoi(argv[2]),
							  atoi(argv[3]), atoi(argv[4]));
	printf("%d %d\n", fib, max);
	return EXIT_SUCCESS;
}
