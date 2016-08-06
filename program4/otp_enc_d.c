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
	socklen_t clilen;
	char msg[BUFFER_SIZE], plaintext_fn[BUFFER_SIZE], key_fn[BUFFER_SIZE];
	char plaintext[BUFFER_SIZE], key[BUFFER_SIZE], ciphertext[BUFFER_SIZE];
	struct sockaddr_in server, client;
	char delim_s = '\x2', delim_e = '\x3';
	int n;

	/* Check number of arguments */
	if (argc < 2)
		exitErr("ERROR: no port provided\n", 1);

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		exitErr("ERROR: creating socket\n", 1);

	/* Configure server */
	memset(&server, '\0', sizeof(server));
	portno = atoi(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);
	server.sin_addr.s_addr = INADDR_ANY;

	/* Bind port to socket */
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
		exitErr("ERROR: binding\n", 1);

	/* Start listening and accepting */
	listen(sockfd, QUEUE_SIZE);
	clilen = sizeof(client);

	while (1) {
//		/* Clean up zombie children and reset buffer */
//		while ((cpid = waitpid(-1, &cstatus, WNOHANG)) > 0) {
//			if (WIFEXITED(cstatus))
//				printf("background pid %d is done: exit value %d\n", cpid, WEXITSTATUS(cstatus));
//		}
////		do {
////			cpid = waitpid(-1, &cstatus, WNOHANG);
////		} while (cpid > 0);
		memset(msg, '\0', BUFFER_SIZE);

		/* Accept new connection */
		client_sockfd = accept(sockfd, (struct sockaddr *) &client, &clilen);
		if (client_sockfd < 0)
			exitErr("ERROR: accepting\n", 1);
//
//		/* Fork child to handle new connection */
//		pid_t spawnpid = fork();
//		if (spawnpid == -1)
//			exitErr("ERROR: forking\n", 1);
//
//		/* In parent: Move on */
//		if (spawnpid > 0)
//			continue;
//
//		/* In child: Continue handling new connection */
//		printf("Server: in child pid %d\n", getpid());

		/* Read plaintext_fn from socket. Attempt to open, read
		 * and scan file for invalid characters. If error
		 * occurs, write error msg to client and move on.
		 */
		safeRead(client_sockfd, plaintext_fn, &delim_s, &delim_e);
		readFile(plaintext_fn, plaintext, msg);
		if (strlen(msg) > 0) {
			safeWrite(client_sockfd, msg, &delim_s, &delim_e);
			continue;
		}

		/* Read key_fn from socket. Attempt to open, read
		 * and scan file for invalid characters. If error
		 * occurs, write error msg to client and move on.
		 */
		safeRead(client_sockfd, key_fn, &delim_s, &delim_e);
		readFile(key_fn, key, msg);
		if (strlen(msg) > 0) {
			safeWrite(client_sockfd, msg, &delim_s, &delim_e);
			continue;
		}

		/* If key shorter than plaintext, write error msg to
		 * client and move on.
		 */
		if (strlen(plaintext) > strlen(key)) {
			sprintf(msg, "ERROR: key '%s' too short\n", key_fn);
			safeWrite(client_sockfd, msg, &delim_s, &delim_e);
			continue;
		}


		/* Encrypt plaintext with key and write ciphertext
		 * to client.
		 */
		encryptText(plaintext, key, ciphertext);
		safeWrite(client_sockfd, ciphertext, &delim_s, &delim_e);


		/* Clean up and exit */
//		close(client_sockfd);
//		close(sockfd);
//		exit(0);
	}

	/* Clean up */
	close(client_sockfd);
	close(sockfd);
	exit(0);
}
