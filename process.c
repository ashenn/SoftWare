#include <stdio.h>
#include "server.h"
#include "process.h"

int destroyClient(process* p){
	p->alive = 0;
}

int swapEnergy(process* p1, process* p2){
	int swap;

	swap = p1->energy;
	p1->energy = p2->energy;
	p2->energy = swap;
}

void addEnergy(process* p, int val){
	p->energy += val;
	
	if (p->energy > 100)
	{
		destroyClient(p);
	}
	else if(p->energy < 0){
		p->energy = 0;
	}
}

int invertDirection(process* p){
	int dir;
	dir = p->direction;

	if (dir == 0)
	{
		dir = 2;
	}
	else if (dir == 1)
	{
		dir = 3;
	}
	else if (dir == 2)
	{
		dir = 0;
	}
	else if (dir == 3)
	{
		dir = 1;
	}

	p->direction = dir;
}

int backward(server* s, process* p){
	if (p->action < 2)
	{
		return 0;
	}

	invertDirection(p);
	forward(s, p);
	invertDirection(p);
	p->action--;
}

int left(process* p){
	if (p->action < 1)
	{
		return 0;
	}

	if (p->direction > 0)
	{
		p->direction--;
	}
	else{
		p->direction = 3;
	}

	p->action--;
}

int right(process* p){
	if (p->action < 1)
	{
		return 0;
	}

	if (p->direction < 3)
	{
		p->direction++;
	}
	else{
		p->direction = 0;
	}

	p->action--;
}

int leftward(server* s, process* p){
	if (p->action < 2)
	{
		return 0;
	}

	left(p);
	forward(s, p);

	return 1;
}

int rightward(server* s, process* p){
	if (p->action < 2)
	{
		return 0;
	}

	right(p);	
	forward(s, p);

	return 1;
}

int forward(server* s, process* p){
	int dir;
	int pos;
	int newPos;
	int size;
	int move;
	process* p2;

	if (p == NULL)
	{
		return -1;
	}

	if (p->action < 1)
	{
		return 0;
	}

	dir = p->direction;
	pos = p->position;
	size = s->size;


	if (dir == 0)
	{
		if (pos < size)
		{
			return 0;
		}

		move = -size;
	}
	else if (dir == 1)
	{
		if (pos > 0 && pos % (size -1) == 0)
		{
			return 0;
		}

		move = 1;
	}
	else if (dir == 2)
	{
		if (pos > (size * size-1))
		{
			return 0;
		}

		move = +size;
	}
	else if (dir == 3)
	{
		if (pos % size == 0)
		{
			return 0;
		}

		move = -1;
	}

	newPos = pos + move;	
	p->action--;

	p2 = getClientAtPos(s, newPos);

	if(p2 != NULL){
		swapEnergy(p, p2);
		return 0;
	}
	else if (s->map[newPos] == CELL_WALL)
	{
		return 0;
	}
	else
	{
		p->position = newPos;
	}

	return 1;
}

int attack(server* s, process* p){
	int i;
	int pos;
	int move;
	int dir;
	int size;
	process* p2;

	if (p == NULL)
	{
		return -1;
	}
	else if (p->action <= 0 || p->energy <= 0)
	{
		return 0;
	}

	p->action--;
	addEnergy(p, -1);

	size = s->size;
	dir = p->direction;
	pos = p->position;

	if (dir == 0)
	{
		move = -size;
	}
	else if (dir == 1)
	{
		move = 1;
	}
	else if (dir == 2)
	{
		move = +size;
	}
	else if (dir == 3)
	{
		move = -1;
	}

	for (i = 0; i < 4; ++i)
	{
		pos += move;
		if (s->map[pos] == CELL_WALL)
		{
			return 1;
		}

		p2 = getClientAtPos(s, pos);
		if (p2 != NULL)
		{
			p2->stun = 2;
			return 1;
		}

		return 1;
	}
}

int gather(server* s, process* p){
	int pos;

	if (p->action < 2)
	{
		return 0;
	}

	pos = p->position;

	if(s->map[pos] == CELL_ENRG){
		s->map[pos] = CELL_EMPTY;
		// addEnergy(p, l)
	}
}

char* selfid(process* p){
	return p->id;
}

int selfstatus(process* p){
	return p->energy;
}

// int inspect(server* s, char* id){
// 	process* p;

// 	p = getClientById(s, id);
// 	if (p == NULL)
// 	{
// 		return -1;
// 	}

// 	return p->energy;
// }

void next(process* p){
	p->wait = 1;
}
