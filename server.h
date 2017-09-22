#ifndef __SERVER_C__
#define __SERVER_C__


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
	int cycle;
	int map_size;
	int game_status; // 0: waiting, 1: started, 2: finished

	int privPort;
	int pubPort;

	Sockets* sockets;

	ListManager* params;
	ListManager* players;
	ListManager* energy_cells;
	ListManager* player_actions;
} GameInfo;

typedef struct Player {
	int id;
	char* name;
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
	int position;
} EnergyCell;


GameInfo* initServer( int argc, char *argv[]);
GameInfo* getServer();
void* Respond(char* msg);
void* Publish(char* msg);
void* NewClent(char* data);
void* HandlePrivate();
Player* getClientAtPos(int pos);
void* printMap();
void *Tick();
void* setMapSize(int size);
void placeWalls();
int initMap();
int createSockets();
void initServArgs();
void initPlayerArgs();
int coord2pos(int x, int y);
int getPosition(Player* p);
void pos2coord(int pos, int res[]);
#endif