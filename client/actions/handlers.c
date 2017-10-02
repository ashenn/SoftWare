#include "../client.h"

void handleString (char *data, void *destination) {
    *(char**)(destination) = data;
    printf("handle string : %s !!\n", data);
    logger->dbg("handle string : %s", data);
}

void handleInt (char *data, void *destination) {
    *(int*)(destination) = str2int(data);
    printf("handle int : %s !!\n", data);
    logger->dbg("handle int");
}

void handleNull (char *data, void *destination) {
    *(int*)(destination) = 1;
    printf("handle null\n");
    logger->dbg("handle null\n");
}