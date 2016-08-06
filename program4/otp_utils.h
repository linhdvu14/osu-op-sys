/*********************************************************************
** otp_utils.h
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 4 (OTP)		Due: 8/10/2016
** Description: Utility functions header.
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

//char delim_s = '\x2', delim_e = '\x3';

void exitErr(const char* msg, int errno);

void safeWrite(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e);
void safeRead(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e);
int delimWrite(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e);
int delimRead(int sockfd, char buffer[BUFFER_SIZE], char* delim_s, char* delim_e);

void readFile(char fn[BUFFER_SIZE], char content[BUFFER_SIZE], char msg[BUFFER_SIZE]);

int getCodeFromChar(char c);
char getCharFromCode(int c);

void encryptText(char intext[BUFFER_SIZE], char outtext[BUFFER_SIZE], char key[BUFFER_SIZE]);
void decryptText(char intext[BUFFER_SIZE], char outtext[BUFFER_SIZE], char key[BUFFER_SIZE]);


#endif /* OTP_UTILS_H_ */
