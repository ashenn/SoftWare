#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../common.h"

int sendMsg(void* requester, char* id, char* act, char* data){
    int len = strlen(id) + strlen(act) + strlen(data) + 3;
    char msg[len];
    sprintf(msg, "%s|%s|%s", id ,act, data);
    logger->inf("Sending: %s", msg);
    zmq_send (requester, msg, 100, 0);
}

int main (int argc, char* argv[])
{
    logger = initLogger(argc, argv);
    srand(time(NULL));

    char uid[5];
    char buffer[100];
    int id = rand() % 99;
    
    

    logger->inf("Creating Context 1 Sockets");
    void *context = zmq_ctx_new ();

    void *public = zmq_socket (context, ZMQ_SUB);
    void *requester = zmq_socket (context, ZMQ_REQ);

    logger->inf("Setting identity\n");
    sprintf(uid, "#0x%d", id);
    logger->inf("UID: %s", uid);
    int rc = zmq_setsockopt(requester, ZMQ_IDENTITY, "azertyu", 8);

    if (rc == -1)
    {
        logger->err("Fail to set identity");
        return 0;
    }

    char test[10];
    size_t size = sizeof(test);
    rc = zmq_getsockopt(requester, ZMQ_IDENTITY, &test, &size);
    
    if (rc == -1){
        logger->err("Fail get identity");
    }
    logger->dbg("indentity: %s", test);

    logger->inf("Connecting to server…\n");
    zmq_connect (public, "tcp://localhost:4343");
    zmq_setsockopt(public, ZMQ_SUBSCRIBE, "B", 1);

    zmq_connect (requester, "tcp://localhost:4242");
    
    logger->inf("Login as ID: %s", uid);

    sendMsg(requester, "", "identify", uid);
    memset(buffer, 0, sizeof(buffer));
    
    zmq_recv (requester, buffer, 100, 0);
    if (strcmp(buffer, "ok")){
        logger->err("Login Fail: %s", buffer);
        return 0;
    }
    memset(buffer, 0, sizeof(buffer));

    logger->inf("Login Success");

    logger->dbg("Enter Loop\n");
    while(1){
        zmq_recv (requester, buffer, 100, 0);
        if (strlen(buffer) > 1)
        {
            logger->inf("Private %s\n", buffer);
            logger->inf("Len: %d\n", (int)strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
        }

        zmq_recv (public, buffer, 100, 0);
        if (strlen(buffer) > 1)
        {
            logger->inf("Public => %s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }

    }

    zmq_close (requester);
    zmq_close (public);
    zmq_ctx_destroy (context);
    return 0;
}