#include "server.h"
// Player Management

void* HandlePrivate();
void initPlayerArgs();
int getPosition(Player* p);
void* NewClent(char* data);
Player* getClientAtPos(int pos);
void* removeEnergy(Player* p, int amt);