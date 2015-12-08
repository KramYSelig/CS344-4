#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	time_t t;
	int keylength,
		i = 0,
		randNum = 0;
	srand((unsigned) time(&t));
	keylength = atoi(argv[1]);
	
	if (argc != 2) {
		fprintf(stderr, "Usage: keygen [keylength]\n");
		exit(1);
	}

	for (i = 0; i < keylength; i++) {
		randNum = (rand() % 27) + 64;
		if (randNum == 64) {
			printf("%c", 32);
		} else {
			printf("%c", randNum);
		}
	}
	printf("\n");

	return 0;
}
