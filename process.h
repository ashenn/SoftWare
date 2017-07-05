#ifndef __PROCESS_H__
#define __PROCESS_H__

typedef struct s_server server;

typedef struct s_process
{
	short isInit;
	char* id;
	int alive;
	int action;
	int num;
	int energy;
	int position;
	int direction; // [0-4]	0:top sens aiguilles montre
	int stun;
	int wait;
} process;

int backward(server* s, process* p);
int forward(server* s, process* p);
int attack(server* s, process* p);

#endif
