/*********************************************************************
** otp_enc_d.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Encryption daemon.
*********************************************************************/

#include "otp_utils.h"


int main(int argc, char *argv[]) {
	int sockfd, client_sockfd, portno, cpid, cstatus;
	char msg[BUFFER_SIZE], buffer[BUFFER_SIZE];
	char intext[BUFFER_SIZE], outtext[BUFFER_SIZE], key[BUFFER_SIZE];
	struct sockaddr_in server, client;
	char delim_s = '\x2', delim_e = '\x3';
	int n;
	int errno = 0;

	/* Check number of arguments */
	if (argc < 2)
		exitErr("ERROR: no port provided\n", 1);

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		exitErr("ERROR: socket()\n", 1);

	/* Configure server */
	memset(&server, '\0', sizeof(server));
	portno = atoi(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);
	server.sin_addr.s_addr = INADDR_ANY;

	/* Bind port to socket */
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
		exitErr("ERROR: bind()\n", 1);

	/* Start listening and accepting */
	listen(sockfd, QUEUE_SIZE);

	while (1) {
		/* Clean up zombie children and reset buffer */
		do {
			cpid = waitpid(-1, &cstatus, WNOHANG);
		} while (cpid > 0);
		memset(msg, '\0', BUFFER_SIZE);

		/* Accept new connection */
		client_sockfd = accept(sockfd, NULL, NULL);
		if (client_sockfd < 0)
			exitErr("ERROR: accept()\n", 1);

		/* Fork child to handle new connection */
		pid_t spawnpid = fork();
		if (spawnpid == -1)
			exitErr("ERROR: fork()\n", 1);

		/* In parent: Move on */
		if (spawnpid > 0)
			continue;

		/* In child: Continue handling new connection */

		/* Validate connection identity. If connection not from
		 * otp_enc, write error msg to client and break.
		 */
		safeRead(client_sockfd, buffer, &delim_s, &delim_e);
		if (strcmp(buffer, "otp_enc") != 0) {
			sprintf(msg, "ERROR: cannot contact server on port %d\n", portno);
			safeWrite(client_sockfd, msg, &delim_s, &delim_e);
			errno = 1;
			break;
		}

		/* Read text and key */
		safeRead(client_sockfd, intext, &delim_s, &delim_e);
		safeRead(client_sockfd, key, &delim_s, &delim_e);

		/* Encrypt text, write result to client,
		 * and exit.
		 */
		encryptText(intext, outtext, key);
		safeWrite(client_sockfd, outtext, &delim_s, &delim_e);
		break;
	}

	/* Clean up */
	close(client_sockfd);
	close(sockfd);
	exit(errno);
}
