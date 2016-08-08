/*********************************************************************
** otp_utils.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Utility functions.
** Sources consulted:
** 	http://stackoverflow.com/questions/19127398/socket-programming-read-is-reading-all-of-my-writes
** 	http://stackoverflow.com/questions/15384518/how-many-bytes-can-i-write-at-once-on-a-tcp-socket
*********************************************************************/

#include "otp_utils.h"


/*********************************************************************
* exitErr()
* Description: Exit with provided errno and write error msg to stderr
* Params:
*	msg: string of error message
*	errno: error number
* Returns: None.
*********************************************************************/
void exitErr(const char* msg, int errno) {
	fprintf(stderr, "%s", msg);
	exit(errno);
}


/*********************************************************************
* safeWrite()
* Description: Loop version of delimWrite()
* Params:
* 	sockfd: file descriptor of socket
* 	buffer: message to be sent
* 	delim_s: delim character signaling STX
*	delim_e: delim character signaling ETX
* Returns: None.
*********************************************************************/
void safeWrite(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e) {
	int n;
	do {
		n = delimWrite(sockfd, buffer, delim_s, delim_e);
	} while (n != 0);
}


/*********************************************************************
* safeRead()
* Description: Loop version of delimRead()
* Params:
* 	sockfd: file descriptor of socket
* 	buffer: buffer to write msg to
* 	delim_s: delim character signaling STX
*	delim_e: delim character signaling ETX
* Returns: None.
*********************************************************************/
void safeRead(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e) {
	int n;
	do {
		n = delimRead(sockfd, buffer, delim_s, delim_e);
	} while (n != 0);
}


/*********************************************************************
* delimWrite()
* Description: Deliminates message and writes to socket. Loops until
* 	full message finishes writing. Terminates if network error.
* Params:
* 	sockfd: file descriptor of socket
* 	buffer: message to be sent
* 	delim_s: delim character signaling STX
*	delim_e: delim character signaling ETX
* Returns: 0 if write successful, 1 otherwise.
*********************************************************************/
int delimWrite(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e) {
	int n;
	int bytes_written = 0;

	/* Write start delim char */
	do {
		n = write(sockfd, delim_s, 1);
		if (n < 0)
			exit(1);
	} while (n != 1);

	/* Write message */
	while (bytes_written != strlen(buffer)) {
		n = write(sockfd, &buffer[bytes_written], strlen(buffer) - bytes_written);
		if (n < 0)
			exit(1);
		bytes_written += n;
	}

	/* Write end delim char */
	do {
		n = write(sockfd, delim_e, 1);
		if (n < 0)
			exit(1);
	} while (n != 1);

	return 0;
}


/*********************************************************************
* delimRead()
* Description: Reads deliminated message and writes to buffer. Loops
* 	until full message read. Terminates if network error or message
* 	not found.
* Params:
* 	sockfd: file descriptor of socket
* 	buffer: buffer to write msg to
* 	delim_s: delim character signaling STX
*	delim_e: delim character signaling ETX
* Returns: 0 if read successful, 1 otherwise.
*********************************************************************/
int delimRead(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e) {
	int n, buflen;
	char c;

	/* Look for start delim char */
	n = read(sockfd, &c, 1);
	if (n < 0 || c != *delim_s)
		exit(1);

	/* Start delim found, reset buffer and read msg */
	memset(buffer, '\0', BUFFER_SIZE);
	buflen = 0;
	do {
		/* Read each char into c */
		do {
			n = read(sockfd, &c, 1);
			if (n < 0)
				exit(1);
		} while (n != 1);

		/* Stop when reach end delim char */
		if (c == *delim_e)
			break;

		/* Update buffer location */
		buffer[buflen] = c;
		buflen++;
	} while (1);

	return 0;
}


/*********************************************************************
* readFile()
* Description: Attempt to open and read content of file (except last
* 	newline character)
* Params:
*	fn: filename to be read
*	content: where to store file content
*	msg: where to store error message
* Returns: None. content and msg will be updated.
*********************************************************************/
void readFile(char fn[BUFFER_SIZE], char content[BUFFER_SIZE], char msg[BUFFER_SIZE]) {
	FILE* file;
	int i;

	/* Reset content and msg */
	memset(msg, '\0', BUFFER_SIZE);
	memset(content, '\0', BUFFER_SIZE);

	/* If error opening file, write error to msg */
	file = fopen(fn, "r");
	if (!(file))
		sprintf(msg, "ERROR: cannot open file '%s'\n", fn);

	/* If error reading file, write error to msg */
	else if (fgets(content, BUFFER_SIZE, file) < 0)
		sprintf(msg, "ERROR: cannot read file '%s'\n", fn);

	/* If invalid characters, write error to msg */
	else {
		strtok(content, "\n");	// strip terminating newline
		for (i = 0; i < strlen(content); i++) {
			if ((content[i] < 65 || content[i] > 90) && content[i] != 32) {
				sprintf(msg, "ERROR: invalid characters in file '%s'\n", fn);
				break;
			}
		}
	}

	/* Clean up */
	if (strlen(msg) > 0)
		memset(content, '\0', BUFFER_SIZE);
	fclose(file);
}


/*********************************************************************
* charToNum()
* Description: Returns code of provided character, so that 'A'
* 	is 0, 'B' is 1, ... 'Z' is 25, ' ' is 26.
* Params:
*	char: uppercase or space characters
* Returns: Numeric code of provided character
*********************************************************************/
int charToNum(char c) {
	if (c == 32)
		return 26;
	return c - 65;
}


/*********************************************************************
* numToChar()
* Description: Returns character corresponding to provided numeric code,
* so that 'A' is 0, 'B' is 1, ... 'Z' is 25, ' ' is 26.
* Params:
*	code: number in range 0-26
* Returns: Character corresponding to provided code
*********************************************************************/
char numToChar(int c) {
	if (c == 26)
		return ' ';
	return c + 65;
}


/*********************************************************************
* encryptText()
* Description: Encrypts input text with provided key
* Params:
*	intext: input plaintext
*	key: key string; must be longer than intext
*	outtext: location to store output ciphertext
* Returns: None. outtext will be updated.
*********************************************************************/
void encryptText(char intext[BUFFER_SIZE], char outtext[BUFFER_SIZE], char key[BUFFER_SIZE]) {
	int i;
	int outcode;

	/* Reset outtext */
	memset(outtext, '\0', BUFFER_SIZE);

	/* Encrypt */
	for (i = 0; i < strlen(intext); i++) {
		outcode = (charToNum(intext[i]) + charToNum(key[i])) % 27;
		outtext[i] = numToChar(outcode);
	}
}


/*********************************************************************
* decryptText()
* Description: Decrypts input text with provided key
* Params:
*	intext: input ciphertext
*	key: key string; must be longer than intext
*	outtext: location to store output plaintext
* Returns: None. outtext will be updated.
*********************************************************************/
void decryptText(char intext[BUFFER_SIZE], char outtext[BUFFER_SIZE], char key[BUFFER_SIZE]) {
	int i;
	int outcode;

	/* Reset outtext */
	memset(outtext, '\0', BUFFER_SIZE);

	/* Decrypt */
	for (i = 0; i < strlen(intext); i++) {
		outcode = (charToNum(intext[i]) - charToNum(key[i]) + 27) % 27;
		outtext[i] = numToChar(outcode);
	}
}
