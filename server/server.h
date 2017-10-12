#ifndef __SERVER_H__
#define __SERVER_H__

#include "../common.h" // Map Management 


typedef struct EnergyCell {
	int id;
	int x;
	int y;
	int value;
	int position;
} EnergyCell;

typedef struct GameInfo {
	char* map;
	int cells_cnt;
	int cycle;
	int map_size;
	int game_status; // 0: waiting, 1: started, 2: finished

	int privPort;
	int pubPort;
	char* curPlayer;

	Sockets* sockets;

	ListManager* params;
	ListManager* players;
	ListManager* energy_cells;
	ListManager* player_actions;
} GameInfo;

#include "map.h" // Map Management 
#include "server_player.h" // Player Management 
#include "server_socket.h"// Socket Managment

// Server Management
void initServArgs();
GameInfo* getServer();
GameInfo* initServer( int argc, char *argv[]);
void closeServer();

// Threads
void *Tick();


#endif