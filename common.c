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