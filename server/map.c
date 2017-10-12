#include "map.h"

void genEnergy(){
    GameInfo* s = getServer();
    short pos = 0;
    short placed = 0;

    int limit = s->cells_cnt *2;
    EnergyCell* cell = malloc(sizeof(EnergyCell));
    Node* n = NULL;
    int co[2];
    int i = 0;
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
        n = add_NodeV(s->energy_cells, "energy", (void*) cell, 1);
        cell->id = n->id;

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

    s->map = malloc(s->cells_cnt +1);

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

EnergyCell* getEnergyAtPos(int pos){
    GameInfo* s = getServer();
    Node* n =  s->energy_cells->first;
    EnergyCell* e;

    do{
        e = (EnergyCell*) n->value;
        if (e->position == pos){
            return e;
        }

        n = n->next;
    }while(n != s->energy_cells->first && n != NULL);

    return NULL;
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

void getVerticalLine(int pos, int dir, int len, int skip, char res[], short stopOnWall){
    GameInfo* s = getServer();
    if (dir <= 1){
        dir = -s->map_size;
    }
    else{
        dir = s->map_size;
    }

    int z;
    int cur;
    int i = 0;
    Player* p;
    for (cur = 0; cur < len; ++cur){
        pos += dir;
        if (cur < skip){
            continue;
        }

        if (pos > s->cells_cnt-1 || pos < 0){
            break;
        }

        
        if (s->map[pos] == CELL_PLAYER){
            p = getClientAtPos(pos);
            if (p == NULL){
                res[i] = '.';
                continue;
            }
            
            for (z = 0; z < 5; ++z){
                res[i+z] = p->name[z];
            }

            i += z-1;
            continue;
        }
        
        if (stopOnWall && s->map[pos] == CELL_WALL){
            break;
        }

        if (i){
            res[i++] = ';';
        }
        res[i++] = s->map[pos];
    }
    
    res[i++] = '\0';
}

void getHorizontalLine(int pos, int dir, int len, int skip, char res[], short stopOnWall){
    GameInfo* s = getServer();
    if (dir <= 1){
        dir = 1;
    }
    else{
        dir = -1;
    }

    int z;
    int cur;
    int i = 0;
    for (cur = 0; cur < len; ++cur){
        pos += dir;
        if (cur < skip){
            continue;
        }
        i++;

        if (pos < 0 || pos > s->cells_cnt){
            break;
        }

        else if (dir > 0 && !(pos % s->map_size)){
            break;
        }
        else if(dir < 0 && !((pos+1) % s->map_size)){
            break;
        }

        if (stopOnWall && s->map[pos] == CELL_WALL){
            break;
        }

        if (i){
            res[i++] = ';';
        }
        res[i++] = s->map[pos];
    }
    
    res[i++] = '\0';
}

void getLine(int pos, int dir, int len, int skip, char res[], short stopOnWall){
    if (dir == UP || dir == DOWN ){
        getVerticalLine(pos, dir, len, skip, res, stopOnWall);
        return;
    }

    getHorizontalLine(pos, dir, len, skip, res, stopOnWall);
    return;
}

void concatVision(char* res, int pos, int look, int* i){
    GameInfo* s = getServer();
    Player* p;

    int z;
    if (!posInBound(pos, s->map_size, look)){
        res[(*i)++] = '0';
    }
    else if(s->map[pos] != CELL_PLAYER){
        res[(*i)++] = s->map[pos];
    }
    else{
        p = getClientAtPos(pos);
        for (z=0; z < strlen(p->name); ++z){
            res[(*i)++] = p->name[z];
        }
    }
    res[(*i)++] = ';';
}

void getVison(Player* p, char* res){
    GameInfo* s = getServer();
    int pos = p->position;
    int look = p->looking;

    int mult;
    int base;
    // int coord[2];
    switch(look){
        case DOWN:
            mult = -1;
            base = s->map_size;
            break;

        case UP:
            mult = 1;
            base = -s->map_size;
            break;

        case LEFT:
            mult = s->map_size;
            base = 1;
            break;

        case RIGHT:
            mult = -s->map_size;
            base = -1;
            break;
    }
    
    // logger->dbg("### Getting Vision for: %s", p->name);
    // logger->inf("-- Direction: %s", getLookName(look));
    // logger->inf("-- pos: %d", p->position);
    // logger->inf("-- base: %d", base);
    // logger->inf("-- mult: %d", mult);


    int i=0;
    int tmpPos;

    // logger->inf("### Cell Calculation");

    tmpPos = pos + base;
    // logger->inf("-- Cell 1: %d | %d => %c", tmpPos, posInBound(tmpPos, s->map_size, p->looking), s->map[tmpPos]); // same

    concatVision(res, tmpPos, p->looking, &i);



    tmpPos = (pos - mult) + (base * 2);
    if (p->looking == UP){
        look = LEFT;
    }
    else if(p->looking == DOWN){
        look = RIGHT;
    }
    else{
        look = p->looking;
    }
    // logger->inf("-- Cell 2: %d | %d => %c", tmpPos, posInBound(tmpPos, s->map_size, look), s->map[tmpPos]); // down left | up right
    concatVision(res, tmpPos, look, &i);


    tmpPos = (pos) + (base * 2);
    // logger->inf("-- Cell 3: %d | %d => %c", tmpPos, posInBound(tmpPos, s->map_size, p->looking), s->map[tmpPos]);
    
    concatVision(res, tmpPos, p->looking, &i);


    tmpPos = (pos + mult) + (base * 2);
    if (p->looking == UP){
        look = RIGHT;
    }
    else if(p->looking == DOWN){
        look = LEFT;
    }
    else{
        look = p->looking;
    }
    // logger->inf("-- Cell 4: %d | %d => %c", tmpPos, posInBound(tmpPos, s->map_size, look), s->map[tmpPos]); // down right | up left
    concatVision(res, tmpPos, look, &i);
    

    res[i] = '\0';
}