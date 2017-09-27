#include "../client.h"

void handleSelfId (char *data, void *destination) {
	destination = data;
    logger->inf("FANTASTIC !!!!, your id is : %s", data);
}

char *sendSelfId () {
    Client *client = getClient();
    char *id = malloc(5);
    sendMsg("selfid", "", &handleSelfId, id);
    return id;
}