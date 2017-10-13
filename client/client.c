#include "client.h"
#include <json-c/json.h>

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


Client* initClient(int argc, char* argv[]){
    static Client *c = NULL;

    if (c != NULL)
        return c;



    c = malloc (sizeof(Client));
    c->player = malloc(sizeof(Player));
    c->sockets = malloc(sizeof(Sockets));
        logger->inf("Creating Context 1 Sockets");

    c->ip = NULL;
    c->pubPort = 0;
    c->priPort = 0;

    initClientArgs();
    parseArgs(c->params, argc, argv);

    if (!c->pubPort) {
        c->pubPort = 4343;
    }   
    if (!c->priPort) {
        c->priPort = 4242;
    } 
    if (c->ip == NULL) {
        c->ip = malloc(16);
        strcpy(c->ip, "127.0.0.1");
    } 

    c->sockets->context = zmq_ctx_new ();
    c->sockets->public = zmq_socket (c->sockets->context, ZMQ_SUB);
    c->sockets->private = zmq_socket (c->sockets->context, ZMQ_REQ);
    c->mapSize = 0;

    c->condition = condition; /* Création de la condition */
    c->mutex = mutex; /* Création du mutex */


     
      char* pub_url = malloc(sizeof(char) * 30);
      char* pri_url = malloc(sizeof(char) * 30);

    sprintf(pub_url, "tcp://%s:%d", c->ip, c->pubPort);    
    
    sprintf(pri_url, "tcp://%s:%d", c->ip, c->priPort);   
    logger->inf("Connecting to server…\npri: %s\npub:%s", pri_url, pub_url); 

    zmq_connect (c->sockets->public, pub_url);
    zmq_setsockopt(c->sockets->public, ZMQ_SUBSCRIBE, "B", 1);
    zmq_connect (c->sockets->private, pri_url);

    logger->inf("Setting identity\n");
    int id = rand() % 99;
    sprintf(c->uid, "#0x%d", id);
    logger->inf("UID: %s", c->uid);

    return c;
}

Client* getClient() {
    return initClient(0, NULL);
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
        logger->inf("Handle PR done");

        memset(packet, 0, sizeof(packet));
        logger->inf("Handle PR memset packet");
    }
}


int sendMsg(char* action, char* data, void (*callback)(char*, void*), void* destination){
    char buffer[100];
    Client *client = getClient();
    int len = strlen(client->uid) + strlen(action) + strlen(data) + 3;
    char msg[len];

    sprintf(msg, "%s|%s|%s", client->uid ,action, data);
    logger->inf("Sending: %s", msg);

    zmq_send (client->sockets->private, msg, 100, 0);
    //    logger->err("Error while sending : %s",  strerror(errno));

    zmq_recv(client->sockets->private, buffer, 100, 0);
    handlePrivateResponse(buffer, callback, destination);

}

void handle(int notif, json_object* data){
    static int i = 0;
    logger->dbg("====== Handeling notif [%d] ======", i++);

    switch (notif){
        case 0:
            logger->inf("#### New Cycle ####");
            logger->dbg("mutexxxxx");

            pthread_mutex_lock (&mutex); /* On verrouille le mutex */
            pthread_cond_signal (&condition); /* On délivre le signal : condition remplie */
            pthread_mutex_unlock (&mutex); /* On déverrouille le mutex */
            
            break;
        case 1:
            logger->inf("#### Game Started ####");
            break;

        case 2:
            logger->inf("#### Game Ended ####");
            break;

        case 3:
            logger->inf("#### A player is dead ####");
            break;

        case 4:
            logger->inf("#### A player haw WON ####");
            break;
    }
    logger->dbg("====== Notif Done [%d] ======", i);
}
 
/*Parsing the json object*/
void json_parse(json_object * jobj) {
    logger->dbg("## Parsing Request##");
    enum json_type type;
    int notifType;
    json_object* data = NULL;

logger->dbg("## lets foreach #");
    json_object_object_foreach(jobj, key, val) { /*Passing through every array element*/
        logger->dbg("-- key: %s", key);
        type = json_object_get_type(val);

        switch (type) {
            case json_type_int: 
                notifType = json_object_get_int(val);
               break; 

            case json_type_object: 
                data = val;
               break; 
        }

    }

    handle(notifType, data);
} 

void* HandleNotif() {
    char buffer[5000];
    char* req[4];
    Client* c = getClient();
    logger->inf("Handle Notif");
    json_object* jobj = NULL;

    logger->inf("Player initialisation");
    c->player->energy = 50;


    while(1){
        memset(buffer, 0, sizeof(buffer));


        zmq_recv (c->sockets->public, buffer, sizeof(buffer), 0);
        
        if (strlen(buffer) > 1){

            jobj = json_tokener_parse(buffer);
            logger->dbg("--parse : %s", buffer);
        
            logger->dbg("--parse");
            json_parse(jobj);
            
            logger->dbg("--free");
            free(jobj);
        }
    }
}

void *setPrivatePort(int val) {
    Client *client = getClient();
    client->pubPort = val;

}
void *setPublicPort(int val) {
    Client *client = getClient();
    client->pubPort = val;
}
void *setIP(char *val) {
    Client *client = getClient();
    client->ip = val;
}



void initClientArgs(){

    Client *client = getClient();

        static Arg arg = {
        .name = "-rep-port", 
        .function = setPrivatePort,
        .hasParam = 1,
        .defParam = NULL, 
        .asInt = 1, 
        .type="num"
    };

    static Arg arg2 = {
        .name = "-pub-port", 
        .function = setPublicPort,
        .hasParam = 1,
        .defParam = NULL, 
        .asInt = 1, 
        .type="num"
    };

    static Arg arg3 = {
        .name = "-ip", 
        .function = setIP,
        .hasParam = 1,
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

  static  Arg* params[] = {
        &arg,
        &arg2,
        &arg3,
        NULL
    };

    client->params = initListMgr();
    client->params = defineArgs(params);
}

void *HandleIA() {
    Client *client = getClient();
    logger->inf("Handle IA thread");
    while (1) { 

        pthread_mutex_lock(&mutex); /* On verrouille le mutex */
        pthread_cond_wait (&condition, &mutex); /* On attend que la condition soit remplie */
        logger->inf("TICK RECU");
        

         aiMakeDecision();
         logger->inf("AI IS DONE");
        pthread_mutex_unlock(&mutex);
        logger->inf("MUTEX UNLOCKED");
    }
}



int main (int argc, char* argv[])
{
    logger = initLogger(argc, argv);
    srand(time(NULL));

   
    ListManager *commands;
    
    Client *client = initClient(argc, argv);

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

    pthread_t ia;
    if (pthread_create(&ia, NULL, HandleIA, coordinates)) {
        logger->err("pthread_create IA");

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

    if (pthread_join(ia, NULL)) {
        logger->err("Fail to Wait For IA");
        return EXIT_FAILURE;
    }

    

    

    logger->inf("Login Success");
-
    zmq_close (client->sockets->private);
    zmq_close (client->sockets->public);
    zmq_ctx_destroy (client->sockets->context);
    
    return 0;
}