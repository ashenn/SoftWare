#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../common.h"

typedef struct Client {
	char uid[5];
	Player *player;
	Sockets *sockets;
	int pubPort;
	int priPort;
	char* ip;
	int mapSize;
	ListManager* params;
	pthread_cond_t condition;; /* Création de la condition */
	pthread_mutex_t mutex;; /* Création du mutex */

} Client;

typedef struct IAMemory {
	int enemyIndex;
	int enemyCoordinates;
	int energyCoordinates;

} IAMemory;

int sendMsg (char* action, char* data, void (*callback)(char*, void*), void* destination);
Client* getClient();


int *sendIdentity ();
char *sendSelfId ();
int sendRight ();
int sendLeft ();
int sendForward ();
int sendBackward ();
int sendRightFwd ();
int sendLeftFwd ();

void handleString (char *data, void *destination);
void handleInt (char *data, void *destination);
void handleNull (char *data, void *destination);

int sendNext ();
int sendAttack ();
int sendGather ();
int sendInspect (char *target);
int sendNext ();
int sendJump ();

int sendSelfStats ();
char *sendWatch ();
char *sendSelfId ();
int sendOrientation ();


void initClientArgs();
void aiMakeDecision();