#include "../client.h"

int sendSelfStats () {
    Client *client = getClient();
    int result = 0;
    sendMsg("selfstats", "", &handleInt, &result);
    return result;
}

char *sendWatch () {
    Client *client = getClient();
    char *result = malloc(25);
    sendMsg("watch", "", &handleString, &result);
    return result;
}

char *sendSelfId () {
    Client *client = getClient();
    char *id = malloc(5);
    sendMsg("selfid", "", &handleString, id);
    return id;
}

int sendOrientation () {
    Client *client = getClient();
    int result = 0;
    sendMsg("looking", "", &handleInt, &result);
    return result;
}