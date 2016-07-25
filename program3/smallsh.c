/*********************************************************************
** smallsh.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 3 (smallish)		Due: 8/1/2016
** Description: Simple shell simulation.
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_NUM_ARGS 512
#define MAX_INPUT_LENGTH 2048


/*********************************************************************
* readInput()
* Description: Reads command line inputs of the form
* 	command [arg1 arg2 ...] [< input_file] [> output_file] [&]
* Params:
* 	commandArgs: destination array for command line inputs
* 	inputFile: redirected input filename
* 	outputFile: redirected output filename
* 	isBackground: should process be started as background?
* Returns: None. The function reads arguments into commandArgs and sets
* 	appropriate values for inputFile, outputFile, isBackground.
*********************************************************************/
void readInput(char** commandArgs, char** inputFile, char** outputFile, int* isBackground) {
	char line[MAX_INPUT_LENGTH];
	int numArgs = 0;
	char* token;
	char* sep = " \t\n";	 // input delimiters

	/* Reset arguments */
	commandArgs[0] = NULL;
	*inputFile = NULL;
	*outputFile = NULL;
	*isBackground = 0;

	/* Read full line of command input */
	fgets(line, MAX_INPUT_LENGTH, stdin);

	/* Process space-separated arguments */
	token = strtok(line, sep);
	while (token) {
		/* Check for input redirection */
		if (strcmp(token, "<") == 0) {
			token = strtok(NULL, sep);
			*inputFile = strdup(token);
		}

		/* Check for output redirection */
		else if (strcmp(token, ">") == 0) {
			token = strtok(NULL, sep);
			*outputFile = strdup(token);
		}

		/* Check for background process */
		else if (strcmp(token, "&") == 0) {
			*isBackground = 1;
		}

		/* Copy other arguments to array commandArgs */
		else {
			commandArgs[numArgs] = strdup(token);
			numArgs++;
		}

		/* Next token */
		token = strtok(NULL, sep);
	}

	/* NULL-terminate array */
	commandArgs[numArgs] = NULL;
}



/*********************************************************************
* catchSigTerm()
* Description: Signal handler that terminates the current process
* 	and returns the signal number.
* Params:
* 	signo: signal number
* Returns: Signal number.
*********************************************************************/
void catchSigTerm(int signo) {
	printf("terminated by signal %d", signo);
	fflush(stdout);
	exit(signo);
}



/*********************************************************************
** MAIN **/

int main() {
	char** commandArgs = malloc(sizeof(char*) * MAX_NUM_ARGS);	// array of arguments
	int isBackground = 0;	// should command execute as background process?
	int exitCalled = 0;		// has exit been called?
	int status = 0;			// status of last foreground process (0, 1, or value of terminating signal)
	int cstatus;			// status of child process (0, 1, or value of terminating signal)
	int cpid;				// child pid
	int fdIn, fdOut, fdIn2, fdOut2;		// file descriptors
	char* inputFile = NULL;				// redirected input filename
	char* outputFile = NULL;			// redirected output filename
	char dir[MAX_INPUT_LENGTH];			// home directory name


	/* Define SIGINT handler which ignores signal by default */
	struct sigaction actINT;
	actINT.sa_flags = 0;
	actINT.sa_handler = SIG_IGN;
	sigfillset(&(actINT.sa_mask));


	/* Loop until user exits */
	while (!exitCalled) {

		/* Clean up zombie children. Print exit value or value of terminating signal
		 * for each terminated child background process.
		 */
		while ((cpid = waitpid(-1, &cstatus, WNOHANG)) > 0) {
			/* Child exits normally */
			if (WIFEXITED(cstatus)) {
				printf("background pid %d is done: exit value %d\n", cpid, WEXITSTATUS(cstatus));
				fflush(stdout);

			/* Child terminated by signal */
			} else if (WIFSIGNALED(cstatus)) {
				printf("background pid %d is done: terminated by signal %d\n", cpid, WTERMSIG(cstatus));
				fflush(stdout);
			}
		}


		/* Display command line prompt and get user input */
		printf(": ");
		fflush(stdout);
		readInput(commandArgs, &inputFile, &outputFile, &isBackground);

		/* If blank line or comment entered, skip to next iteration */
		if (!commandArgs[0] || strncmp(commandArgs[0], "#", 1) == 0)
			continue;

		/* Handle built-in "exit" */
		if (strcmp(commandArgs[0], "exit") == 0) {
			exitCalled = 1;
			continue;
		}


		/* Handle built-in "status" */
		if (strcmp(commandArgs[0], "status") == 0) {
			/* Last foreground exits normally */
			if (status == 0 | status == 1) {
				printf("exit value %d\n", status);
				fflush(stdout);

			/* Last foreground terminated by signal */
			} else {
				printf("terminated by signal %d\n", status);
				fflush(stdout);
			}
			status = 0;
			continue;
		}


		/* Handle built-in "cd" */
		if (strcmp(commandArgs[0], "cd") == 0) {
			/* No directory specified. Change to home directory */
			if (!commandArgs[1])	// no dir specified; change to home
				sprintf(dir, "%s", getenv("HOME"));

			/* Absolute path */
			else if (strncmp(commandArgs[1], "/", 1) == 0)
				sprintf(dir, "%s", commandArgs[1]);

			/* Relative path */
			else
				sprintf(dir, "%s/%s", getenv("HOME"), commandArgs[1]);

			/* Change dir */
			if (chdir(dir) == -1) {
				perror("cd");
				exit(1);
			}
			continue;
		}


		/* Handle non-built-in commands by spawning child */
		pid_t spawnpid = fork();
		if (spawnpid == -1) {
			perror("fork");
			exit(1);
		}


		/* In child: Execute non-built-in commands here */
		else if (spawnpid == 0) {
			/* Set up input redirection. If no input specified for
			 * background process, redirect input from /dev/null.
			 */
			if (isBackground && !inputFile)
				inputFile = "/dev/null";
			if (inputFile) {
				fdIn = open(inputFile, O_RDONLY);
				if (fdIn == -1) {
					printf("cannot open %s for input\n", inputFile);
					fflush(stdout);
					exit(1);
				}
				fdIn2 = dup2(fdIn, 0);
				if (fdIn2 == -1) {
					perror("dup2");
					exit(1);
				}
			}

			/* Set up output redirection. If no output specified for
			 * background process, redirect output to /dev/null.
			 */
			if (isBackground && !outputFile)
				outputFile = "/dev/null";
			if (outputFile) {
				fdOut = open(outputFile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
				if (fdOut == -1) {
					printf("cannot open %s for output\n", outputFile);
					fflush(stdout);
					exit(1);
				}
				fdOut2 = dup2(fdOut, 1);
				if (fdOut2 == -1) {
					perror("dup2");
					exit(1);
				}
			}

			/* Set up SIGINT handler. Foreground process should terminate.
			 * Background process should ignore signal.
			 */
			if (!isBackground)
				actINT.sa_handler = catchSigTerm;
			sigaction(SIGINT, &actINT, NULL);

			/* Execute command. Print error message for foreground process. */
			execvp(commandArgs[0], commandArgs);
			if (!isBackground) {
				printf("%s: No such file or directory\n", commandArgs[0]);
				fflush(stdout);
			}
			exit(1);
		}


		/* In parent: Clean up after children processes here */
		else if (spawnpid > 0) {
			/* Set up SIGINT handler. Parent should ignore signal. */
			sigaction(SIGINT, &actINT, NULL);

			/* Print child pid if background process */
			if (isBackground) {
				printf("background pid is %d\n", spawnpid);
				fflush(stdout);
			}

			/* Wait for child and update status value
			 * if foreground process.
			 */
			if (!isBackground) {
				waitpid(spawnpid, &cstatus, 0);
				
				/* Child exits normally */
				if (WIFEXITED(cstatus)) {
					status = WEXITSTATUS(cstatus);

				/* Child terminated by signal */
				} else if (WIFSIGNALED(cstatus)) {
					status = WTERMSIG(cstatus);
					printf("terminated by signal %d\n", status);
					fflush(stdout);
				}			
			}
		}
	}


	/* User called exit. Clean up memory and terminate all spawned processes. */
	free(commandArgs);
	if (kill(0, SIGTERM))
		exit(0);
	else
		exit(1);

}

