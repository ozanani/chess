#include <string.h>
#include <stdio.h>
#include "ConsoleGame.h"
#include "GraphicalGame.h"

int main(int argc, char * argv[]) {
	int error = 0;

	// no parameters - console game
	if (argc == 1) graphicalGameRun();

	// first parameter is -g or -c
	else if (argc == 2) {
		if (strcmp(argv[1], "-c") == 0) consoleGameRun();
		else if (strcmp(argv[1], "-g") == 0) graphicalGameRun();

		// wrong parameter
		else error = 1;
	}

	// more than one param
	else error = 1;

	if (error) {
		printf("USAGE: %s [-g / -c]\n", argv[0]);
		return 1;
	}

	return 0;
}