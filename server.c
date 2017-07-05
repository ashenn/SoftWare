#include <stdlib.h> 
#include <stdio.h>
#include <time.h>

#include "lib.h"
#include "server.h"

int initServer(server* s){
	int i;

	s->players = malloc(4 * sizeof(process));
	for (i = 0; i < 4; ++i)
	{
		s->players[i].isInit = 0;
	}
	
	if(initMap(s) == -1){
		return 0;
	}

	s->cycle = 0;
	s->start = 0;

	printf("SERVER INIT\n");
	return 1;
}

void placeWalls(server* s){
	int i;
	int obs;
	int pos;
	int size;
	int ratio;

	size = s->size;
	ratio = (size * 100) / 99;

	// obs = rand() % (ratio) -1;
	obs = ratio;

	for (i = 0; i < s->max; ++i)
	{
		s->map[i] = CELL_EMPTY;
	}

	for (i = 0; i < obs; ++i)
	{
		pos = 0;
		while(pos == 0 || pos == size-1 || pos == size * (size-1) || pos == (size * size) -1){
			pos = rand() % ((size * size)- 1);
		}


		printf("placing Wall: %d\n", pos);
		s->map[pos] = CELL_WALL;
	}
}

int setMapSize(server* s){
	int res;
	int size;
	int max;

	size = s->size;
	res = 1;
	if (size > 21 || size < 7)
	{
		size = 10;
		res = 0;
	}


	max = size * size;
	s->size = size;
	s->max = max;
	printf("mapsize: %d\n", s->max);

	s->map = malloc(max);

	if (s->map == NULL)
	{
		printf("Fail to malloc\n");
		return -1;
	}

	s->map[max] = '\0';

	return res;
}

void getClientPos(server* s,int* res){
	int i;
	process* p;

	for (i = 0; i < 4; ++i)
	{
		p = &s->players[i];
		if (p->isInit)
		{
			res[i] = p->position;
		}
		else{
			res[i] = -1;
		}
	}
}

process* getClientAtPos(server* s,int pos){
	int i;
	process* p;

	for (i = 0; i < 4; ++i)
	{
		p = &s->players[i];
		
		if (p->isInit && p->position == pos)
		{
			return p;
		}
	}

	return NULL;
}

process* getClientByNum(server* s,int i){
	process* p;
	if (i < 0 || i > 3)
	{
		return NULL;
	}

	return &s->players[i];
}

process* getClientById(server* s, char* id){
	int i;

	for (i = 0; i < 4; ++i)
	{
		if (strComp(s->players[i].id, id))
		{
			return &s->players[i];
		}
	}

	return NULL;	
}

void printMap(server* s){
	int a;
	int i;
	int j;
	int size;
	process* p;

	size = s->size;

	j = 0;
	for (i = 0; i != s->max; ++i, ++j)
	{
		if (j == 0)
		{
			printf("|");
		}

		p = getClientAtPos(s, i);
		if (p != NULL)
		{
			s->map[i] = p->num;
		}
		else if (s->map[i] != CELL_WALL)
		{
			s->map[i] = CELL_EMPTY;
		}

		if (p != NULL)
		{
			printf("%s|", p->id);
		}
		else{
			for (a = 0; a < 5; ++a)
			{
				printf("%c", s->map[i]);
			}
			printf("|");
		}

		if (j == size-1)
		{
			printf("\n");
			j=-1;
		}
	}
}

void clearScreen(){
	int i;
	for (i = 0; i < 50; ++i)
	{
		print("\n");
	}
}

int initMap(server* s){
	if(setMapSize(s) == -1){
		return -1;
	}

	placeWalls(s);
}

int idExists(server* s, char* id){
	int i;

	for (i = 0; i < 4; ++i)
	{
		if (strComp(id, s->players[i].id))
		{
			return 1;
		}
	}

	return 0;
}

char* getNewId(server* s){
	int i;
	int id;
	char* idstr;
	char* idstrTmp;

	id = rand() % 100;
	idstrTmp = int2str(id, 2);
	
	idstr = strConcat("#0x", idstrTmp);
	
	if (idExists(s, idstr))
	{
		return getNewId(s);
	}

	return idstr;
}

int addClient(server* s){
	printf("adding client:\n");
	int i;
	int id;
	int size;
	char* idstr;
	short added;
	process* p;

	size = (s->size) - 1;
	added = -1;
	for (i = 0; i < 4; ++i)
	{
		p = &s->players[i];

		if (p->isInit == 0)
		{
			s->players[i].id = getNewId(s);
			if (i==0)
			{
				p->position = 0;
				p->direction = 2;
			}
			else if(i==1){
				p->position = size;
				p->direction = 2;
			}
			else if(i==2){
				p->position = ((size) * (size+1));
				p->direction = 0;
			}
			else{
				p->position = s->max - 1;
				p->direction = 0;
			}

			p->num = i;
			p->energy = 50;
			p->action = 2;
			p->wait = 0;

			p->alive = 1;
			p->isInit = 1;
			added = i;
			printf("id: %s\n", p->id);
			printf("pos: %d\n", p->position);
			break;
		}
	}

	return added;
}

// int cellContent(server* s, int pos){
// 	char cell;

// 	if (pos < 0 || pos >= max)
// 	{
// 		return -1;
// 	}

// 	cell = s->map[pos];

// }

void preCycle(server* s){
	int i;
	process* p;

	// clearScreen();
	// printf("cycle: %d\n", s->cycle);
	// printf("pos: %d\n", s->players[0].position);
	// printf("nrg: %d\n", s->players[0].energy);
	// printf("stun: %d\n", s->players[2].stun);

	for (i = 0; i < 4; ++i)
	{
		p = &s->players[i];
		if (p->alive)
		{
			addEnergy(p, -2);
			p->action = 2;
			if (p->stun > 0)
			{
				p->stun--;
			}
		}
	}
}

void postCycle(server* s){
	// printMap(s);
	usleep(400000);
}

void cycle(server* s){
	forward(s, &s->players[0]);
	// attack(s, &s->players[0]);
}

void runGame(server* s){
	for (s->cycle; s->cycle < 10000; ++s->cycle){
		preCycle(s);
		cycle(s);
		postCycle(s);
	}
}


void* startServer(void * p_data){
	int i;
	server* s;

	s = (server*) p_data;
	// for (i = 0; i < 4; ++i)
	// {
	// 	addClient(s);
	// }

	while(s->start != 1){
		usleep(200);
	}
	runGame(s);
}