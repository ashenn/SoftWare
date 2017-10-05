#ifndef __COMMON_H__
#define __COMMON_H__

#define CELL_ENRG '@'
#define CELL_WALL '#'
#define CELL_EMPTY '.'
#define CELL_PLAYER '+'

#define STATE_WAIT 0
#define STATE_START 1
#define STATE_FINISH 0

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define TURN_RIGHT 1
#define TURN_LEFT -1

#define PORT_PRIV 4242
#define PORT_PUB 4343
#define CYCLE 500000


#include <assert.h>

#include <stdio.h>
#include <time.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#include <zmq.h>
#include "zhelpers.h"

#include "base/basic.h"
#include "base/libList.h"
#include "base/libParse.h"
#include "base/logger.h"

typedef struct Sockets{
	void* context;
    void* public;
    void* private;
} Sockets;

typedef struct Player {
	int id;
	char* name;
	int x;
	int y;
	int position;
	int energy;
	int action;
	int stuned;
	int looking; // 0: up, 1: right, 2: down, 3: left;
} Player;

char* getLookName(int look);
int coord2pos(int x, int y, int map_size);
void pos2coord(int pos, int map_size, int res[]);
short posInBound(int pos, int map_size, int dir);

// int coord2pos(int x, int y, int map_size){
//     return x + (y * map_size);
// }

// void pos2coord(int pos, int map_size, int res[]){
//     res[1] = pos / map_size;
//     res[0] = pos - (res[1] * map_size);
// }

#endif