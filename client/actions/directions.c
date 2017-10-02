#include "../client.h"

int sendRight () {
    Client *client = getClient();
    int result = 0;
    sendMsg("right", "", &handleNull, &result);
    return result;
}

int sendLeft () {
    Client *client = getClient();
    int result = 0;
    sendMsg("left", "", &handleNull, &result);
    return result;
}

int sendRighFwd () {
    Client *client = getClient();
    int result = 0;
    sendMsg("rightfwd", "", &handleNull, &result);
    return result;
}

int sendLeftFwd () {
    Client *client = getClient();
    int result = 0;
    sendMsg("leftfwd", "", &handleNull, &result);
    return result;
}

int sendForward () {
    Client *client = getClient();
    int result = 0;
    sendMsg("forward", "", &handleNull, &result);
    return result;
}

int sendBackward () {
    Client *client = getClient();
    int result = 0;
    sendMsg("backward", "", &handleNull, &result);
    return result;
}
