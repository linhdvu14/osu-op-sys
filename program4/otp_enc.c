/*********************************************************************
** otp_enc.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Encryption.
*********************************************************************/

#include "otp_utils.h"




int main(int argc, char *argv[]) {
	int sockfd, portno, n, idx;
	struct sockaddr_in server;
	struct hostent *server_ip_addr;

	char buffer[BUFFER_SIZE];
	char plaintext_fn[BUFFER_SIZE], key_fn[BUFFER_SIZE];
	char c;
	char delim_s = '\x2', delim_e = '\x3';


	/* Check number of arguments */
	if (argc < 4) {
		fprintf(stderr, "Usage: %s plaintext key port\n", argv[0]);
		exit(1);
	}

	/* Read info from command args */
	portno = atoi(argv[3]);
	strcpy(plaintext_fn, argv[1]);
	strcpy(key_fn, argv[2]);


	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		exitErr("ERROR: creating socket\n");


	/* Configure server */
	server_ip_addr = gethostbyname("localhost");
	memset(&server, '\0', sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);
	memcpy(&server.sin_addr, server_ip_addr->h_addr, server_ip_addr->h_length);


	/* Connect to server */
	if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		fprintf(stderr, "ERROR: connecting to server\n");
		exit(1);
	}


	/* Write plaintext_fn to socket */
	do {
		n = writeSocketDelim(sockfd, plaintext_fn, &delim_s, &delim_e);
	} while (n != 0);

	/* Read plaintext from socket */
	do {
		n = readSocketDelim(sockfd, buffer, &delim_s, &delim_e);
	} while (n != 0);
	printf("Client: receive plaintext '%s'\n", buffer);


	/* Write key_fn to socket */
	do {
		n = writeSocketDelim(sockfd, key_fn, &delim_s, &delim_e);
	} while (n != 0);


	/* Read key from socket */
	do {
		n = readSocketDelim(sockfd, buffer, &delim_s, &delim_e);
	} while (n != 0);
	printf("Client: receive key '%s'\n", buffer);


	/* Read ciphertext from socket */
	do {
		n = readSocketDelim(sockfd, buffer, &delim_s, &delim_e);
	} while (n != 0);
	printf("Client: receive ciphertext '%s'\n", buffer);


	/* Read plaintext from socket */
	do {
		n = readSocketDelim(sockfd, buffer, &delim_s, &delim_e);
	} while (n != 0);
	printf("Client: receive plaintext '%s'\n", buffer);


	/* Clean up */
	close(sockfd);
	return 0;

}
