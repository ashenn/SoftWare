#include "../client.h"

void handleDirection (char *data, void *destination) {
    *(int*)(destination) = 1;
    logger->inf("DIRECTION CHANGED !!");
}

int sendRight () {
    Client *client = getClient();
    int result = 0;
    sendMsg("right", "", &handleDirection, &result);
    return result;
}

int sendLeft () {
    Client *client = getClient();
    int result = 0;
    sendMsg("left", "", &handleDirection, &result);
    return result;
}

int sendUp () {
    Client *client = getClient();
    int result = 0;
    sendMsg("up", "", &handleDirection, &result);
    return result;
}

int sendDown () {
    Client *client = getClient();
    int result = 0;
    sendMsg("down", "", &handleDirection, &result);
    return result;
}
