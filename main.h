#ifndef __MAIN_C__
#define __MAIN_C__


#define CELL_WALL '#'
#define CELL_EMPTY '.'
#define CELL_ENRG '@'

#define STATE_WAIT 0
#define STATE_START 1
#define STATE_FINISH 0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Sockets{
	void* context;
    void* public;
    void* private;
} Sockets;

typedef struct GameInfo {
	char* map;
	int cells_cnt;
	int map_size;
	int game_status;

	Sockets* sockets;

	ListManager* players;
	ListManager* energy_cells;
} GameInfo;

typedef struct Player {
	int id;
	char * name;
	int x;
	int y;
	int position;
	int energy;
	int looking;
} Player;

typedef struct EnergyCell {
	int x;
	int y;
	int value;
} EnergyCell;


#endif