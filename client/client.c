#include "client.h"

int sendMsg(void* requester, char* id, char* act, char* data){
    int len = strlen(id) + strlen(act) + strlen(data) + 3;
    char msg[len];
    sprintf(msg, "%s|%s|%s", id ,act, data);
    logger->inf("Sending: %s", msg);
    if (zmq_send (requester, msg, 100, 0) != 0)
        logger->err("Error while sending : %s",  strerror(errno));

}

Client* getClient() {
    static Client *c = NULL;

    if (c != NULL)
        return c;

    c = malloc (sizeof(Client));
    c->player = malloc(sizeof(Player));
    c->sockets = malloc(sizeof(Sockets));
        logger->inf("Creating Context 1 Sockets");

    c->sockets->context = zmq_ctx_new ();
    c->sockets->public = zmq_socket (c->sockets->context, ZMQ_SUB);
    c->sockets->private = zmq_socket (c->sockets->context, ZMQ_REQ);

    logger->inf("Connecting to server…\n");  

    zmq_connect (c->sockets->public, "tcp://localhost:4343");
    zmq_setsockopt(c->sockets->public, ZMQ_SUBSCRIBE, "B", 1);
    zmq_connect (c->sockets->private, "tcp://localhost:4242");

    logger->inf("Setting identity\n");
    int id = rand() % 99;
    sprintf(c->uid, "#0x%d", id);
    logger->inf("UID: %s", c->uid);

    return c;
}

void* HandleResponse() {
    logger->inf("\n== Private Thread Launched ==");
    char buffer[100];
    char* req[4];
    Client* c = getClient();

    while(1){
        memset(buffer, 0, sizeof(buffer));
        zmq_recv(c->sockets->private, buffer, 100, 0);
        
        if (strlen(buffer) > 1){
            logger->dbg("Private2 => %s", buffer);
            // rc = zmq_getsockopt(s->sockets->private, ZMQ_IDENTITY, &test, &size);
            
            memset(req, 0, sizeof(req));
            logger->dbg("exlode");
            explode('|', buffer, 0, 5, req);

            logger->dbg("Id: %s", req[0]);
            logger->dbg("Action: %s", req[1]);
            logger->dbg("data: %s", req[2]);

            memset(buffer, 0, sizeof(buffer));
        }

        sleep(1);
    }
}

void* HandleNotif() {
    char buffer[100];
    char* req[4];
    Client* c = getClient();

    while(1){
        memset(buffer, 0, sizeof(buffer));
        zmq_recv(c->sockets->private, buffer, 100, 0);
        
        if (strlen(buffer) > 1){
            logger->dbg("Private2 => %s", buffer);
            // rc = zmq_getsockopt(s->sockets->private, ZMQ_IDENTITY, &test, &size);
            
            memset(req, 0, sizeof(req));
            logger->dbg("exlode");
            explode('|', buffer, 0, 5, req);

            logger->dbg("Id: %s", req[0]);
            logger->dbg("Action: %s", req[1]);
            logger->dbg("data: %s", req[2]);

            memset(buffer, 0, sizeof(buffer));
        }

        sleep(1);
    }
}

int main (int argc, char* argv[])
{
    logger = initLogger(argc, argv);
    srand(time(NULL));

   
    ListManager *commands;
    
    Client *client = getClient();

    char buffer[100];
    logger->inf("Login as ID: %s", client->uid);

    sendMsg(client->sockets->private, "", "identify", client->uid);
    memset(buffer, 0, sizeof(buffer));

    pthread_t handle;
    if (pthread_create(&handle, NULL, HandleResponse, NULL)) {
        logger->err("pthread_create Private");
        return EXIT_FAILURE;
    }
    


    pthread_t sub;
    if (pthread_create(&sub, NULL, HandleNotif, NULL)) {
        logger->err("pthread_create Tick");

        return EXIT_FAILURE;
    }

    if (pthread_join(sub, NULL)) {
        logger->err("Fail to Wait For Tick");
        return EXIT_FAILURE;
    }

    
    zmq_recv (client->sockets->private, buffer, 100, 0);
    if (strcmp(buffer, "ok")){
        logger->err("Login Fail: %s", buffer);
        return 0;
    }
    memset(buffer, 0, sizeof(buffer));

    logger->inf("Login Success");
/*
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
    */
    zmq_close (client->sockets->private);
    zmq_close (client->sockets->public);
    zmq_ctx_destroy (client->sockets->context);
    
    return 0;
}