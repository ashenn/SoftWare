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
} Client;

int sendMsg (char* action, char* data, void (*callback)(char*, void*), void* destination);
Client* getClient();


int *sendIdentity ();
char *sendSelfId ();
int sendRight ();
int sendLeft ();
int sendForward ();
int sendBackward ();


void handleString (char *data, void *destination);
void handleInt (char *data, void *destination);
void handleNull (char *data, void *destination);

int sendNext ();