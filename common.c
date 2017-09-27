#include "common.h"

int coord2pos(int x, int y, int map_size){
    return x + (y * map_size);
}

void pos2coord(int pos, int map_size, int res[]){
    res[1] = pos / map_size;
    res[0] = pos - (res[1] * map_size);
}

char* getLookName(int look){
    static char* names[] = {
        "UP",
        "RIGHT",
        "DOWN",
        "LEFT",
        NULL
    };

    return names[look];
}

short posInBound(int pos, int map_size, int dir){
    if (pos > map_size * map_size || pos < 0){
        return 0;
    }

    if (dir == LEFT && !(pos % map_size)){
        return 1;
    }

    if (dir == RIGHT && !((pos+1) % map_size)){
        return 0;
    }

    return 1;
}