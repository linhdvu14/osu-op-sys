/*********************************************************************
** otp_utils.h
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Header for utilities files.
** Sources consulted:
** 	http://stackoverflow.com/questions/19127398/socket-programming-read-is-reading-all-of-my-writes
** 	http://stackoverflow.com/questions/15384518/how-many-bytes-can-i-write-at-once-on-a-tcp-socket
*********************************************************************/

#ifndef OTP_UTILS_H_
#define OTP_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>

#define BUFFER_SIZE 4096
#define QUEUE_SIZE 5

void exitErr(const char* msg);
int writeSocketDelim(int sockfd, char* msg, char* delim_s, char* delim_e);
int readSocketDelim(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e);
void readFile(char fn[BUFFER_SIZE], char content[BUFFER_SIZE], char msg[BUFFER_SIZE]);
int getCodeFromChar(char c);
char getCharFromCode(int c);
void encryptText(char plaintext[BUFFER_SIZE], char key[BUFFER_SIZE], char ciphertext[BUFFER_SIZE]);
void decryptText(char plaintext[BUFFER_SIZE], char key[BUFFER_SIZE], char ciphertext[BUFFER_SIZE]);



#endif /* OTP_UTILS_H_ */
