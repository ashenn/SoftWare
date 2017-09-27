#include "server.h"

int initMap();
void genEnergy();
void* printMap();
void placeWalls();
void* setMapSize(int size);
void getLine(int pos, int dir, int len, char res[], short stopOnWall);
EnergyCell* getEnergyAtPos(int pos);
void getVison(Player* p);