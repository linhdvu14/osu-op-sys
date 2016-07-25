/*********************************************************************
** vuli.adventure.c
** Author: Linh Vu
** Course: CS340-400 (Operating Systems), Summer 2016
** Programming Assignment 2 (adventure)		Due: 7/18/2016
** Description: Mini-simulation of Colossal Cave Adventure.
*********************************************************************/


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 512
#define NAME_LENGTH 50
#define MAX_PATH_LENGTH 1000

typedef struct Room Room;


/* Room struct */
struct Room {
	char name[NAME_LENGTH];		// room name
	char type[NAME_LENGTH];		// room type
	char connections[6][NAME_LENGTH];	// names of connections
	int conNum;			// number of connections
};



/*********************************************************************
* shuffle()
* Description: Shuffles the first n elements of an array.
* Params:
* 		array: string array of size > n
* 		n: number of elements to be shuffled
* Returns: None.
* Adapted from: http://www.geeksforgeeks.org/shuffle-a-given-array/
*********************************************************************/
void shuffle(char* array[], int n) {
	int i, j;
	for (i=n-1; i>0; i--) {
		j = rand() % (i+1);
		char* temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}
}


/*********************************************************************
* generateRooms()
* Description: Generates 7 random rooms with the required specs and
* 		writes each to its own file in directory vuli.rooms.[pid].
* Params: None.
* Returns: None.
*********************************************************************/
void generateRooms() {
	Room roomArray[7];
	int i, j;
	FILE* file;

	/* Arrays of possible room names and types */
	char* nameArray[] = {"Room of Requirement", "Chamber of Secrets",
			"Forbidden Forest", "Platform 9 3/4", "St Mungo's",
			"Dungeons", "Hogsmeade", "4 Privet Drive",
			"Diagon Alley", "Knockturn Alley" };

	char* typeArray[] = {"START_ROOM", "END_ROOM", "MID_ROOM",
			"MID_ROOM", "MID_ROOM", "MID_ROOM", "MID_ROOM"};

	/* Randomly assign unique names from nameArray. After shuffling,
	 * nameArray[0..6] will give names of the 7 rooms picked.
	 */
	shuffle(nameArray, 10);
	for (i=0; i<7; i++)
		strcpy(roomArray[i].name, nameArray[i]);


	/* Randomly assign room types from typeArray */
	shuffle(typeArray, 7);
	for (i=0; i<7; i++)
		strcpy(roomArray[i].type, typeArray[i]);


	/* Create 7x7 adjacency matrix to represent room connections.
	 * conMatrix[i][j] = conMatrix[j][i] = 1 if rooms i and j
	 * are connected, 0 otherwise. Sum of conMatrix[i] is total
	 * number of connections of room i.
	 * All rooms are originally unconnected:
	 * 		conMatrix[i][j] = 0 for all i, j.
	 */
	int conMatrix[7][7];
	for (i=0; i<7; i++) {
		for (j=0; j<7; j++)
			conMatrix[i][j] = 0;
	}


	/* For each room, generate a random number of connections between 3-6.
	 * If room currently has less than that many connections, generate
	 * random outgoing connections to reach that number.
	 * Make sure not to connect a room to itself or a room it's already
	 * connected to.
	 * Also make sure to create cross connections, i.e. if room i is connected
	 * to room j, then room j should be connected to room i.
	 */
	for (i=0; i<7; i++) {
		int conNum = 3 + rand() % 4;
		int conCount = 0;			// how many existing connections?
		for (j=0; j<7; j++) {
			if (conMatrix[i][j] == 1)
				conCount++;
		}
		while (conCount < conNum) {
			int j = rand() % 7;		// random connection to room j
			if (j != i && conMatrix[j][i] == 0) {
				conMatrix[i][j] = 1;	// hook i to j
				conMatrix[j][i] = 1;	// hook j to i
				conCount++;
			}
		}
	}


	/* Make a directory vuli.rooms.[processID], and write
	 * each room to a file room[i].txt in that directory.
	 * The file content has format:
	 * 		ROOM NAME: [...]
	 * 		CONNECTION 1: [...]
	 * 		CONNECTION 2: [...]
	 * 		CONNECTION n: [...]
	 * 		ROOM TYPE: [...]
	 */
	char dirname[80];			// create directory
	sprintf(dirname, "%s%d", "vuli.rooms.", getpid());
	if (mkdir(dirname, 0755) < 0)
		exit(1);

	for (i=0; i<7; i++) {	// create 7 room files
		char filename[80];
		sprintf(filename, "%s/room%d.txt", dirname, i);
		file = fopen(filename, "w");
		fprintf(file, "ROOM NAME: %s\n", roomArray[i].name);
		int conCount = 0;
		for (j=0; j<7; j++) {
			if (conMatrix[i][j] == 1) {
				fprintf(file, "CONNECTION %d: %s\n", conCount+1, roomArray[j].name);
				conCount++;
			}
		}
		fprintf(file, "ROOM TYPE: %s\n", roomArray[i].type);
		fclose(file);
	}
}



/*********************************************************************
* readRooms()
* Description: Read room info from files into Room array.
* Params:
* 		array: pointer to Room array of length 7.
* Returns: None.
*********************************************************************/
void readRooms(Room* array) {
	int i;
	FILE* file;
	char dirname[80];
	sprintf(dirname, "%s%d", "vuli.rooms.", getpid());
	for (i=0; i<7; i++) {
		array[i].conNum = 0;
		char filename[80];
		sprintf(filename, "%s/room%d.txt", dirname, i);

		/* Open file */
		file = fopen(filename, "r");
		char word[BUFFER_SIZE];


		/* Bypass first 11 chars ("ROOM NAME: ") to get room name.
		 * Note fgets() reads newline character, so strip it before
		 * assigning.
		 */
		fseek(file, 11, SEEK_CUR);
		fgets(word, BUFFER_SIZE, file);
		strcpy(array[i].name, strtok(word, "\n"));


		/* Read next 11 chars (either "CONNECTION " or "ROOM TYPE: ")
		 * Depending on whether it's connection or type, set the
		 * appropriate properties for current room.
		 */
		while (fgets(word, 12, file)) {
			/* If current line is connection */
			if (strcmp(word, "CONNECTION ") == 0) {
				fseek(file, 3, SEEK_CUR);	// bypass next 3 chars ("[i]: ")
				fgets(word, BUFFER_SIZE, file);
				strcpy(array[i].connections[array[i].conNum], strtok(word, "\n"));
				array[i].conNum++;
			}
			/* If current line is room type */
			else {
				fgets(word, BUFFER_SIZE, file);
				strcpy(array[i].type, strtok(word, "\n"));
			}
		}
		fclose(file);
	}
}



/*********************************************************************
** MAIN **/

int main() {
	srand(time(0));
	int i, j;
	Room roomArray[7];
	Room curRoom;			// current room
	int success = 0;		// has player won?
	int pathLength = 0;		// how many steps till success?
	char path[MAX_PATH_LENGTH][NAME_LENGTH];	// track path to success
	char word[NAME_LENGTH];	// to hold temp stdin

	/* Generate rooms and write to files */
	generateRooms();

	/* Read rooms from files into Room array */
	readRooms(roomArray);

	/* Start game at starting room */
	for (i=0; i<7; i++) {
		if (strcmp(roomArray[i].type, "START_ROOM") == 0) {
			curRoom = roomArray[i];
			break;
		}
	}

	/* Repeat until reach ending room */
	while (!success && pathLength < MAX_PATH_LENGTH) {

		/* Print prompts */
		printf("\nCURRENT LOCATION: %s\n", curRoom.name);
		printf("POSSIBLE CONNECTIONS: ");
		for (i=0; i<curRoom.conNum-1; i++)
			printf("%s, ", curRoom.connections[i]);
		printf("%s.\n", curRoom.connections[curRoom.conNum-1]);
		printf("WHERE TO? >");

		/* Get user input. If input is legal (entered room is
		 * a connection of current room), search room array
		 * and update current room.
		 */
		fgets(word, NAME_LENGTH, stdin);
		strtok(word, "\n");
		int legal = 0;
		for (i=0; i<curRoom.conNum; i++) {
			if (strcmp(word, curRoom.connections[i]) == 0) {
				legal = 1;
				for (j=0; j<7; j++) {
					if (strcmp(word, roomArray[j].name) == 0) {
						curRoom = roomArray[j];
						break;
					}
				}
				break;
			}
		}

		/* If input is legal, store entered room in path,
		 * increment path length and check whether end
		 * room has been reached. If reached, break
		 * out of while loop.
		 * If input is illegal, continue without
		 * incrementing path length.
		 */
		if (legal) {
			if (strcmp(curRoom.type, "END_ROOM") == 0)
				success = 1;
			strcpy(path[pathLength], curRoom.name);
			pathLength++;
		} else
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
	}


	/* Ending room reached. Print success message, path and path length */
	if (pathLength < MAX_PATH_LENGTH) {
		printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
		printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", pathLength);
		for (i=0; i<pathLength; i++)
			printf("%s\n", path[i]);	
	
	/* MAX_PATH_LENGTH reached. Notify user that they lost */
	} else {
		printf("\nYOU TOOK TOO LONG TO FIND THE END ROOM. BYE!\n");
	}

	return 0;

}

