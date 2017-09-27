#include "client.h"

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


void* handlePrivateResponse(char *packet, void (*callback)(char*, void*), void* destination) {
    char* req[2];
        
    if (strlen(packet) > 1){
        logger->dbg("Private2 => %s", packet);
            // rc = zmq_getsockopt(s->sockets->private, ZMQ_IDENTITY, &test, &size);
            
        memset(req, 0, sizeof(req));
        logger->dbg("exlode");
        explode('|', packet, 0, 3, req);

        logger->dbg("status: %s", req[0]);
        logger->dbg("data: %s", req[1]);

        if (strcmp(req[0], "ok")) {
            logger->err("Server returned ko");
            destination = NULL;
            return NULL;
        }

        callback(req[1], destination);

        memset(packet, 0, sizeof(packet));
    }
}


int sendMsg(char* action, char* data, void (*callback)(char*, void*), void* destination){
    char buffer[100];
    Client *client = getClient();
    int len = strlen(client->uid) + strlen(action) + strlen(data) + 3;
    char msg[len];

    sprintf(msg, "%s|%s|%s", client->uid ,action, data);
    logger->inf("Sending: %s", msg);

    if (zmq_send (client->sockets->private, msg, 100, 0) != 0)
        logger->err("Error while sending : %s",  strerror(errno));

    zmq_recv(client->sockets->private, buffer, 100, 0);
    handlePrivateResponse(buffer, callback, destination);

}
 


void* HandleNotif() {
    char buffer[100];
    char* req[4];
    Client* c = getClient();

    while(1){
        memset(buffer, 0, sizeof(buffer));
        
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

void initPlayerActions(){
    logger->dbg("- Init Player Args");
    Client *client = getClient();

    /*static Arg arg1 = {
        .name = "identify", 
        .function = NewClent, 
        .hasParam = 1, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg2 = {
        .name = "right", 
        .function = right, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg3 = {
        .name = "left", 
        .function = right, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg4 = {
        .name = "selfid", 
        .function = selfid, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static  Arg* player_actions[] = {
        &arg1,
        &arg2,
        &arg3,
        &arg4,
        NULL
    };

    srv->player_actions = malloc(sizeof(ListManager));
    srv->player_actions = defineArgs(player_actions);*/
}

int main (int argc, char* argv[])
{
    logger = initLogger(argc, argv);
    srand(time(NULL));

   
    ListManager *commands;
    
    Client *client = getClient();

    char buffer[100];
    logger->inf("Login as ID: %s", client->uid);

    memset(buffer, 0, sizeof(buffer));

    int *coordinates = sendIdentity ();
    if (coordinates == NULL) {
        logger->inf("identify fail");
    }

    char *uidFromServer = sendSelfId ();
    if (coordinates == NULL) {
        logger->inf("selfid fail");
    }

    int test = sendRight();
    if  (test == 0)
        logger->err("right foire");

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
-
    zmq_close (client->sockets->private);
    zmq_close (client->sockets->public);
    zmq_ctx_destroy (client->sockets->context);
    
    return 0;
}