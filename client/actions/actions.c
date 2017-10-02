#include "../client.h"

int sendGather () {
    Client *client = getClient();
    int result = 0;
    sendMsg("gather", "", &handleInt, &result);
    return result;
}

int sendAttack () {
    Client *client = getClient();
    int result = 0;
    sendMsg("attack", "", &handleInt, &result);
    return result;
}

int sendInspect (char *target) {
    Client *client = getClient();
    int result = 0;
    sendMsg("inspect", target, &handleInt, &result);
    return result;
}

int sendNext () {
    Client *client = getClient();
    int result = 0;
    sendMsg("next", "", &handleNull, &result);
    return result;
}

int sendJump () {
    Client *client = getClient();
    int result = 0;
    sendMsg("jump", "", &handleNull, &result);
    return result;
}