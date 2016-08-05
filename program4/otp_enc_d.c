/*********************************************************************
** otp_enc_d.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Encryption daemon.
*********************************************************************/

#include "otp_utils.h"



int main(int argc, char *argv[]) {
	int sockfd, client_sockfd, portno;
	socklen_t clilen;
	char msg[BUFFER_SIZE], plaintext_fn[BUFFER_SIZE], key_fn[BUFFER_SIZE];
    char plaintext[BUFFER_SIZE], key[BUFFER_SIZE], ciphertext[BUFFER_SIZE];
	struct sockaddr_in server, client;
	char c;
    char delim_s = '\x2', delim_e = '\x3';
	int n, i, idx;

	/* Check number of arguments */
	if (argc < 2)
		exitErr("ERROR: no port provided\n");

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		exitErr("ERROR: creating socket\n");

	/* Configure server */
    memset(&server, '\0', sizeof(server));
	portno = atoi(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);
	server.sin_addr.s_addr = INADDR_ANY;

	/* Bind port to socket */
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
		exitErr("ERROR: binding\n");

	/* Start listening and accepting */
    listen(sockfd, QUEUE_SIZE);
	clilen = sizeof(client);

	while (1) {
		memset(msg, '\0', BUFFER_SIZE);

		client_sockfd = accept(sockfd, (struct sockaddr *) &client, &clilen);
		if (client_sockfd < 0)
			exitErr("ERROR: accepting\n");

		/* Read plaintext_fn from socket */
		do {
			n = readSocketDelim(client_sockfd, plaintext_fn, &delim_s, &delim_e);
		} while (n != 0);
		printf("Server: receive plaintext_fn '%s'\n", plaintext_fn);


		/* Attempt to read plaintext. If there is error message,
		 * write error to client and move on.
		 */
		readFile(plaintext_fn, plaintext, msg);
	    if (strlen(msg) > 0) {
			do {
				n = writeSocketDelim(client_sockfd, msg, &delim_s, &delim_e);
			} while (n != 0);
		 	continue;
	    }


	    /* Read back plaintext to client */
		do {
			n = writeSocketDelim(client_sockfd, plaintext, &delim_s, &delim_e);
		} while (n != 0);


		/* Read key_fn from socket */
		do {
			n = readSocketDelim(client_sockfd, key_fn, &delim_s, &delim_e);
		} while (n != 0);
		printf("Server: receive key_fn '%s'\n", key_fn);


		/* Attempt to read key. If there is error message,
		 * write error to client and move on.
		 */
		readFile(key_fn, key, msg);
	    if (strlen(msg) > 0) {
			do {
				n = writeSocketDelim(client_sockfd, msg, &delim_s, &delim_e);
			} while (n != 0);
		 	continue;
	    }


	    /* If key shorter than plaintext, write error msg to client and move on */
	    if (strlen(plaintext) > strlen(key)) {
	    	sprintf(msg, "ERROR: key too short");
			do {
				n = writeSocketDelim(client_sockfd, msg, &delim_s, &delim_e);
			} while (n != 0);
		 	continue;
	    }


	    /* Read back key to client */
		do {
			n = writeSocketDelim(client_sockfd, key, &delim_s, &delim_e);
		} while (n != 0);


	    /* Encrypt plaintext with key */
	    encryptText(plaintext, key, ciphertext);

	    /* Read back ciphertext to client */
		do {
			n = writeSocketDelim(client_sockfd, ciphertext, &delim_s, &delim_e);
		} while (n != 0);

	    /* Decrypt ciphertext with key */
	    decryptText(plaintext, key, ciphertext);

	    /* Read back plaintext to client */
		do {
			n = writeSocketDelim(client_sockfd, plaintext, &delim_s, &delim_e);
		} while (n != 0);





	}

 	/* Clean up */
 	close(client_sockfd);
    close(sockfd);
    return 0;
}




