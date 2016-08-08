/*********************************************************************
** otp_dec.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Decryption interface.
*********************************************************************/

#include "otp_utils.h"


int main(int argc, char *argv[]) {
	int sockfd, portno, n, idx;
	struct sockaddr_in server;
	struct hostent *server_ip_addr;

	char buffer[BUFFER_SIZE], msg[BUFFER_SIZE];
	char text_fn[BUFFER_SIZE], key_fn[BUFFER_SIZE];
	char text[BUFFER_SIZE], key[BUFFER_SIZE];
	char delim_s = '\x2', delim_e = '\x3';

	/* Check number of arguments */
	if (argc < 4) {
		fprintf(stderr, "Usage: %s plaintext key port\n", argv[0]);
		exit(1);
	}

	/* Read command args */
	strcpy(text_fn, argv[1]);
	strcpy(key_fn, argv[2]);
	portno = atoi(argv[3]);
	
	/* Open text file and scan for invalid characters.  
	 * If error, write error msg to stderr and exit.
	 */
	if (readFile(text_fn, text, msg) != 0)
		exitErr(msg, 1);	
		
	/* Open key file and scan for invalid characters.  
	 * If error, write error msg to stderr and exit.
	 */
	if (readFile(key_fn, key, msg) != 0)
		exitErr(msg, 1);
	
	/* If key shorter than text, write error msg  
	 * to stderr and exit.
	 */
	if (strlen(text) > strlen(key)) {
		sprintf(msg, "ERROR: key '%s' too short\n", key);
		exitErr(msg, 1);	
	}

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		exitErr("ERROR: socket()\n", 2);

	/* Configure server */
	server_ip_addr = gethostbyname("localhost");
	memset(&server, '\0', sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);
	memcpy(&server.sin_addr, server_ip_addr->h_addr, server_ip_addr->h_length);

	/* Connect to server */
	if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
		exitErr("ERROR: cannot contact server\n", 2);

	/* Write identity to server */
	strcpy(buffer, "otp_dec");
	safeWrite(sockfd, buffer, &delim_s, &delim_e);

	/* Write text and key to socket */
	safeWrite(sockfd, text, &delim_s, &delim_e);
	safeWrite(sockfd, key, &delim_s, &delim_e);

	/* Read from socket and print out error msg or resulting text
	 * as appropriate.
	 */
	safeRead(sockfd, buffer, &delim_s, &delim_e);
	if (strncmp(buffer, "ERROR: cannot contact server on port ", 37) == 0)
		exitErr(buffer, 2);
	
	printf("%s\n", buffer);

	/* Clean up */
	close(sockfd);
	exit(0);
}
