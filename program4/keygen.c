/*********************************************************************
** keygen.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Random key generator.
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]) {
	int i, idx;
	srand(time(0));

	/* Check that two arguments are supplied */
	if (argc != 2) {
		printf("Usage: keygen numchars\n");
		exit(1);
	}

	/* Print random characters from list of allowed chars */
	char chars[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	for (i=0; i<atoi(argv[1]); i++) {
		idx = rand() % 27;
		printf("%c", chars[idx]);
	}
	printf("\n");

	exit(0);
}

