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