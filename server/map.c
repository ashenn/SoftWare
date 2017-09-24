#include "map.h"

void genEnergy(){
    GameInfo* s = getServer();
    short pos = 0;
    short placed = 0;

    int limit = s->cells_cnt *2;
    EnergyCell* cell;

    int co[2];
    do{
        pos = (rand() % (s->cells_cnt -2)) + 1;


        pos2coord(pos, s->map_size, co);

        if (getClientAtPos(pos)){
            continue;
        }

        if (s->map[pos] == CELL_ENRG){
            continue;
        }

        if (s->map[pos] == CELL_WALL){
            continue;
        }

        s->map[pos] = CELL_ENRG;
        
        cell->x = co[0];
        cell->y = co[1];
        cell->position = pos;

        cell->value = 5 + (rand() % 10);
        add_NodeV(s->energy_cells, "energy", (void*) cell);

        placed=1;
    }while(!placed && --limit > 0);

    if (!limit){
        logger->dbg("Fail to place Energy");
        return;
    }

    logger->dbg("Energy Placed At Pos: %d", pos);
}

void* printMap(){
    logger->dbg("printig");
    int a;
    int i;
    int j;
    int size;
    Player* p = NULL;
    GameInfo* s = getServer();

    size = s->map_size;
    j = 0;
    for (i = 0; i != s->cells_cnt; ++i, ++j)
    {
        if (j == 0)
        {
            fprintf(stdout, "|");
        }

        if (s->map[i] == CELL_PLAYER)
        {
            p = getClientAtPos(i);
            fprintf(stdout, "%s|", p->name);
        }
        else{
            for (a = 0; a < 5; ++a) {
                fprintf(stdout, "%c", s->map[i]);
            }
            fprintf(stdout, "|");
        }

        if (j == size-1)
        {
            fprintf(stdout, "\n");
            j=-1;
        }
    }
}


void* setMapSize(int size){
    logger->inf("    == Set Map Size (%d) ==", size);
    GameInfo* s = getServer();
    
    if (size > 21 || size < 5)
    {
        logger->err("     -Map %d not in Range (5, 21) Size set to Default (%d) ", size, 5);
        size = 5;
    }

    s->map_size = size;
    s->cells_cnt = size * size;
    logger->dbg("      mapsize: %d", s->cells_cnt);

    s->map = malloc(s->cells_cnt);

    if (s->map == NULL)
    {
        logger->err("      Fail to malloc");
        return NULL;
    }

    s->map[s->cells_cnt] = '\0';
}

void placeWalls(){
    logger->inf("    == Place Walls ==");
    int i;
    int obs;
    int pos;
    int size;
    int ratio;
    GameInfo* s = getServer();

    size = s->map_size;
    ratio = (size * 100) / 99;
    logger->dbg("      -Walls Ratio: %d", ratio);

    // obs = rand() % (ratio) -1;
    obs = ratio;

    for (i = 0; i < s->cells_cnt; ++i)
    {
        s->map[i] = CELL_EMPTY;
    }

    for (i = 0; i < obs; ++i)
    {
        pos = 0;
        while(pos == 0 || pos == size-1 || pos == size * (size-1) || pos == (size * size) -1){
            pos = rand() % ((size * size)- 1);
        }


        logger->dbg("      -Wall #%d: %d", i, pos);
        s->map[pos] = CELL_WALL;
    }

    logger->inf("    == Walls Done ==");
}


int initMap(){
    logger->inf("  == Init Map ==");
    GameInfo* s = getServer();
    if (s->map_size == 0){
        logger->dbg("    -Map size not Set Setting to default");
        setMapSize(5);
    }

    placeWalls();
    logger->inf("  == Map Done ==");
}