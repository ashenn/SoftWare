#ifndef __SERVER_H__
#define __SERVER_H__

#include "../common.h" // Map Management 
#include "map.h" // Map Management 
#include "server_player.h" // Player Management 
#include "server_socket.h"// Socket Managment


typedef struct EnergyCell {
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

	Sockets* sockets;

	ListManager* params;
	ListManager* players;
	ListManager* energy_cells;
	ListManager* player_actions;
} GameInfo;

// Server Management
void initServArgs();
GameInfo* getServer();
GameInfo* initServer( int argc, char *argv[]);

// Threads
void *Tick();


#endif