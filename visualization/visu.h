#ifndef __VISU_H__
#define __VISU_H__

#include "../common.h"


typedef struct Visu {
	int size;
	int status;
	Sockets* sockets;

	ListManager* walls;
	ListManager* players;
	ListManager* cells;
} Visu;

#endif