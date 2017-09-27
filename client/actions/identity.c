#include "../client.h"

void handleIdentity (char *data, void *destination) {
    char* mapDatas[2];
    int coordinates[2];
    explode(';', data, 0, 3, mapDatas);

    printf("%s -> %d !!\n", mapDatas[1], str2int(mapDatas[1]));
    
    pos2coord(str2int(mapDatas[0]), str2int(mapDatas[1]), destination);

    logger->inf("CONNECTED !!!!, you are in {%d, %d}", ((int*)destination)[0], ((int*)destination)[1]);
}

int *sendIdentity () {
    Client *client = getClient();
    int *coordinates = malloc(sizeof(int)*2);
    sendMsg("identify", client->uid, &handleIdentity, coordinates);
    return coordinates;
}