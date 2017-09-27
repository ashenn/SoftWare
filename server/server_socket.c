#include "server_socket.h"

int createSockets(){
    GameInfo* s = getServer();
    s->sockets->context = zmq_ctx_new();
    s->sockets->private = zmq_socket(s->sockets->context, ZMQ_REP);
    s->sockets->public = zmq_socket(s->sockets->context, ZMQ_PUB);

    char* url = malloc(sizeof(char) * 13);
    sprintf(url, "tcp://*:%d", s->privPort);    
    logger->inf("setting priv port to: %s", url);
    int rc = zmq_bind(s->sockets->private, url);

    sprintf(url, "tcp://*:%d", s->pubPort);    
    logger->inf("setting pub port to: %s", url);
    int pc = zmq_bind(s->sockets->public, url);

    free(url);
    assert(rc == 0);
    assert(pc == 0);

    return 1;
}

void* Respond(char* txt, ...){
    GameInfo* s = getServer();
    char msg[100];
    
    va_list args;
    va_start(args, txt);
    va_end(args);

    vsprintf(msg, txt, args);

    logger->inf("Respondig: %s", msg);
    
    zmq_send(s->sockets->private, msg, 100, 0);
}

void* Publish(char* txt, ...){
    char msg[1000];
    GameInfo* s = getServer();
    
    va_list args;
    va_start(args, txt);
    va_end(args);

    vsprintf(msg, txt, args);

    logger->inf("Publishing: %s", msg);
    
    s_sendmore(s->sockets->public, "B");
    s_send(s->sockets->public, msg);
}


void* setPrivatePort(int p){
    if (p < 0){
        logger->war("Private port must be > 0\n port set: to %D", PORT_PRIV);
        p = PORT_PRIV;
    }

    GameInfo* s = getServer();
    s->privPort = p;
}

void* setPublicPort(int p){
    if (p < 0){
        logger->war("Private port must be > 0\n port set: to %d", PORT_PUB);
        p = PORT_PUB;
    }

    GameInfo* s = getServer();
    s->pubPort = p;
}
