/*********************************************************************
** otp_utils.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Utilities files.
** Sources consulted:
** 	http://stackoverflow.com/questions/19127398/socket-programming-read-is-reading-all-of-my-writes
** 	http://stackoverflow.com/questions/15384518/how-many-bytes-can-i-write-at-once-on-a-tcp-socket
*********************************************************************/


#include "otp_utils.h"



/*********************************************************************
* exitErr()
* Description: Exit with code 1 and write error msg to stderr
* Params:
*	msg: string of error message
* Returns: None.
*********************************************************************/
void exitErr(const char* msg) {
	fprintf(stderr, "%s", msg);
	exit(1);
}


/*********************************************************************
* writeSocketDelim()
* Description: Deliminates message and writes to socket. Loops until
* 	full message finishes writing. Terminates if network error.
* Params:
* 	sockfd: file descriptor of socket
* 	msg: message to be sent
* 	delim_s: delim character signaling STX
*	delim_e: delim character signaling ETX
* Returns: 0 if write successful, 1 otherwise.
*********************************************************************/
int writeSocketDelim(int sockfd, char* msg, char* delim_s, char* delim_e) {
	int n;
	int bytes_written = 0;

	/* Write starting delim char */
	do {
		n = write(sockfd, delim_s, 1);
		if (n < 0)
			exit(1);
	} while (n != 1);

	/* Write message */
	while (bytes_written != strlen(msg)) {
		n = write(sockfd, &msg[bytes_written], strlen(msg) - bytes_written);
		if (n < 0)
			exit(1);
		bytes_written += n;
	}

	/* Write ending delim char */
	do {
		n = write(sockfd, delim_e, 1);
		if (n < 0)
			exit(1);
	} while (n != 1);

	return 0;
}



/*********************************************************************
* readSocketDelim()
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
int readSocketDelim(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e) {
	int n, buflen;
	char c;

	/* Look for starting delim */
	n = read(sockfd, &c, 1);
	if (n < 0 || c != *delim_s)
		exit(1);

	/* Start delim found, read msg into buffer */
	memset(buffer, '\0', BUFFER_SIZE);
	buflen = 0;
	do {
		/* Read each char into c */
		do {
			n = read(sockfd, &c, 1);
			if (n < 0)
				exit(1);
		} while (n != 1);

		/* Stop when reach ending delim */
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
    	sprintf(msg, "ERROR: opening file %s", fn);

    /* If error reading file, write error to msg */
    else if (fgets(content, BUFFER_SIZE, file) < 0)
    	sprintf(msg, "ERROR: reading file %s", fn);

    /* If invalid characters, write error to msg */
    else {
    	strtok(content, "\n");	// strip terminating newline
	    for (i = 0; i < strlen(content); i++) {
	    	if ((content[i] < 65 || content[i] > 90) && content[i] != 32) {
	        	sprintf(msg, "ERROR: invalid characters in file %s", fn);
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
* getCodeFromChar()
* Description: Returns code of provided character, so that 'A'
* 	is 0, 'B' is 1, ... 'Z' is 25, ' ' is 26.
* Params:
*	char: uppercase or space characters
* Returns: Numeric code of provided character
*********************************************************************/
int getCodeFromChar(char c) {
	if (c == 32)
		return 26;
	return c - 65;
}


/*********************************************************************
* getCharFromCode()
* Description: Returns character corresponding to provided numeric code,
* so that 'A' is 0, 'B' is 1, ... 'Z' is 25, ' ' is 26.
* Params:
*	code: number in range 0-26
* Returns: Character corresponding to provided code
*********************************************************************/
char getCharFromCode(int c) {
	if (c == 26)
		return ' ';
	return c + 65;
}




/*********************************************************************
* encryptText()
* Description: Encrypts plaintext with provided key
* Params:
*	plaintext: plaintext string
*	key: key string; must be longer than plaintext
*	ciphertext: location to store encrypted plaintext
* Returns: None. ciphertext will be updated.
*********************************************************************/
void encryptText(char plaintext[BUFFER_SIZE], char key[BUFFER_SIZE], char ciphertext[BUFFER_SIZE]) {
	int i;
	int ciphercode;

	/* Reset ciphertext */
	memset(ciphertext, '\0', BUFFER_SIZE);

	/* Encrypt */
	for (i = 0; i < strlen(plaintext); i++) {
		ciphercode = (getCodeFromChar(plaintext[i]) + getCodeFromChar(key[i])) % 27;
		ciphertext[i] = getCharFromCode(ciphercode);
//		printf("Encrypting char %d. Plain: %c (%d). Key: %c (%d). *Cipher: %c (%d)*\n", i,
//				plaintext[i], getCodeFromChar(plaintext[i]),
//				key[i], getCodeFromChar(key[i]),
//				ciphertext[i], ciphercode);
	}
}


/*********************************************************************
* decryptText()
* Description: Decrypts ciphertext with provided key
* Params:
* plaintext: location to store decrypted ciphertext
*	ciphertext: ciphertext string
*	key: key string; must be longer than plaintext
* Returns: None. plaintext will be updated.
*********************************************************************/
void decryptText(char plaintext[BUFFER_SIZE], char key[BUFFER_SIZE], char ciphertext[BUFFER_SIZE]) {
	int i;
	int plaincode;

	/* Reset plaintext */
	memset(plaintext, '\0', BUFFER_SIZE);

	/* Decrypt */
	for (i = 0; i < strlen(ciphertext); i++) {
		plaincode = (getCodeFromChar(ciphertext[i]) - getCodeFromChar(key[i]) + 27) % 27;
//		printf("Decrypting char %d. *Plain: %c (%d)*. Key: %c (%d). Cipher: %c (%d)\n", i,
//				plaintext[i], plaincode,
//				key[i], getCodeFromChar(key[i]),
//				ciphertext[i], getCodeFromChar(ciphertext[i]));
		plaintext[i] = getCharFromCode(plaincode);
	}
}



