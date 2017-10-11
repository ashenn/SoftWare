#include "../client.h"

IAMemory* getMemory() {
    static IAMemory *mem = NULL;

    if (mem != NULL)
        return mem;

    mem = malloc (sizeof(IAMemory));
    mem->energyCoordinates = -1;
    mem->enemyCoordinates = -1;

    return mem;
}


int getVisionDataCoordinate(int index, int look) {
	index++;
	Client *client = getClient();

	int pos = client->player->position;

    int mult, base, tmpPos;
    // int coord[2];
    switch(look){
        case DOWN:
            mult = -1;
            base = client->mapSize;
            break;

        case UP:
            mult = 1;
            base = -client->mapSize;
            break;

        case LEFT:
            mult = client->mapSize;
            base = 1;
            break;

        case RIGHT:
            mult = -client->mapSize;
            base = -1;
            break;
    }

    switch(index){
        case 1:
            tmpPos = pos + base;
            break;

        case 2:
            tmpPos = (pos + mult) + (base * 2);
            break;

        case 3:
            tmpPos = (pos) + (base * 2);
            break;

        case 4:
            tmpPos = (pos - mult) + (base * 2);
            break;
    }



    logger->inf("-- Cell: %d | %d => %c", tmpPos, posInBound(tmpPos, client->mapSize, look)); // same

    return tmpPos;
	
}

int isInFront(int index) {
	if (index == 1 | index == 3)
		return 1;
	return 0;
}

int isOnSide(int index) {
	if (index == 2 ) //left
		return 1;
	if (index == 4 ) //right
		return 2;
	return 0;
}



void aiMakeDecision() {
	logger->inf("IA IN ACTION !!");

	Client *client = getClient();
	IAMemory *memory = getMemory();

	int looking = sendOrientation();
	int energy = sendSelfStats();

	char *vision = sendWatch();
	char *visionDatas[10];
	explode(';', vision, 0, 4, visionDatas);

	int seeEnemy = 0;

	for (int i=0; i < 4; i++) {
		int position = getVisionDataCoordinate(i, looking);
		if (strlen(visionDatas[i]) == 0) {
			
		}
		else if (strlen(visionDatas[i]) > 1) {
			seeEnemy = 1;
			memory->enemyCoordinates = position;
			memory->enemyIndex = i;
		}
		else {
			if (visionDatas[i][0] == CELL_ENRG)
				memory->energyCoordinates = position;
			if (visionDatas[i][0] == CELL_WALL) {
				//wall
			}
			if (visionDatas[i][0] == CELL_EMPTY) {

			}
		}

	}

	if (seeEnemy == 1) {
		logger->inf("IA: Ennemy detected !");
		if (energy > 20) {
			logger->inf("IA: I have enough energy, attack strategy !");
			if (isInFront(memory->enemyIndex)){
				logger->inf("IA: Enemy in front, I attack him !");
				sendAttack();
			}
			else  {
		 		int side = isOnSide(memory->enemyIndex);
		 		if (side != 0)
		 			logger->inf("IA: Enemy on side, moving to aim him");
				if (side == 1) 
			 		sendLeftFwd();
				if (side == 2)
			 		sendRightFwd();

			}
		}
		else {
			logger->inf("IA: I have NOT enough energy, defense strategy !");
			if (isInFront(memory->enemyIndex) == 1) {
				logger->inf("IA: Enemy in front, moving on side !");
				if (sendLeftFwd() == 0)
					sendRightFwd();
			}
			else  {
		 		int side = isOnSide(memory->enemyIndex);
		 		if (side != 0)
		 			logger->inf("IA: Enemy on side, moving on opposite way");
				if (side == 1)
			 		sendLeftFwd();
				if (side == 2)
			 		sendRightFwd();
			}
		}
		
	} else {
		logger->inf("IA: I see no ennemy !");
		if (energy < 20) {
			logger->inf("IA: I sam hungry, can I eat !");
			if (memory->enemyCoordinates == client->player->position) {
				logger->inf("IA: Oh, I am on an energy cell, gathering it !");
				sendGather();
			}
			else {
				logger->inf("IA: No energy here, let's move !");
				sendForward();
			}
		} else {
			logger->inf("IA: I'm not really hungry, let's move !");
			if (sendForward() == 0)
				if (sendRightFwd() ==0)
					if (sendLeftFwd() == 0)
						return;

		} 		
	}

	logger->inf("IA: Logic is done, see you in next tick ..");
}