#ifndef __SERVER_H__
#define __SERVER_H__
#include <pthread.h>
#include "main.h"
#include "process.h"

#define CELL_WALL '#'
#define CELL_EMPTY '.'
#define CELL_ENRG '@'

typedef struct s_server
{
	pthread_t pid;
	int size;
	int max;
	int cycle;
	char* map;
	int start;
	process* players;
	client* clients;
} server;

process* getClientByNum(server* s,int i);
process* getClientAtPos(server* s,int pos);
void printMap(server* s);
void* startServer(void * p_data);
int addClient(server* s);

#endif
