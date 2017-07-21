#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <time.h> 


#include "zhelpers.h"
#include "libList.h"

#include "basic.h"
#include "libParse.h"
#include "logger.h"
#include "server.h"


GameInfo* initServer(int argc, char *argv[]){
    static GameInfo* s = NULL;

    if (s != NULL){
        return s;
    }

    logger->inf("== Init Server ==");

    s = malloc(sizeof(GameInfo));
    if (s == NULL){
        logger->err("Error: Faild to malloc Server");
        return NULL;
    }

    logger->dbg("-Setting params");
    s->map = NULL;
    s->map_size = 0;
    s->game_status = 0;
    
    initServArgs();
    parseArgs(s->params, argc, argv);


    logger->dbg("-Malloc Sockets");
    s->sockets = malloc(sizeof(Sockets));
    createSockets();
    if (s->sockets == NULL){
        logger->err("Error: Faild to malloc Sockets");
        free(s);
        return NULL;
    }

    logger->dbg("-Malloc Players");
    s->players = initListMgr();
    if (s->players == NULL){
        logger->err("Error: Faild to malloc Players");
        free(s);
        return NULL;
    }

    logger->dbg("-Malloc Energy");
    s->energy_cells = initListMgr();
    if (s->energy_cells == NULL){
        logger->err("Error: Faild to malloc Energy");
        free(s);
        return NULL;
    }


    initMap(s);

    logger->inf("== Server Done ==");
    return s;
}

GameInfo* getServer(){
    return initServer(0, NULL);
}

// void *Tick(void *arg){
//     pthread_exit(NULL);
// }

void* Respond(char* msg){
    GameInfo* s = getServer();
    zmq_send(s->sockets->private, msg, 100, 0);
}

void* Publish(char* msg){
    GameInfo* s = getServer();
    s_sendmore(s->sockets->public, "B");
    s_send(s->sockets->public, msg);
}

int NewClent(char data[]){
    GameInfo* s = getServer();
    if (s->players->nodeCount >= 4){
        Respond("ko");
        return 0;
    }

    logger->dbg("adding client:");
    Node* n = malloc(sizeof(Node));

    
    return 1;
}

void* HandlePrivate(){
    char buffer[100];
    GameInfo* s = getServer();

    while(1){
        memset(buffer, 0, sizeof(buffer));
        zmq_recv(s->sockets->private, buffer, 10, 0);
        
        if (strlen(buffer) > 1){
            logger->dbg("Private => %s", buffer);

            if (!strcmp(buffer, "New Co")){
                logger->dbg("New Connection");
                NewClent(buffer);

                memset(buffer, 0, sizeof(buffer));
            }
        }

        sleep(1);
    }
}


Player* getClientAtPos(int pos){
    int i;
    GameInfo* s = getServer();

    int ids[s->players->nodeCount];
    getIds(s->players, ids);
    Node* n;
    Player* p;

    for (i = 0; i < s->players->nodeCount; ++i)
    {
        n = getNode(s->players, ids[i]);
        p = (Player*) n->value;
        if (p->position == pos)
        {
            return p;
        }
    }

    return NULL;
}


void* printMap(){
    int a;
    int i;
    int j;
    int size;
    Player* p = NULL;
    GameInfo* s = getServer();

    size = s->map_size;

    j = 0;
    for (i = 0; i != s->cells_cnt; ++i, ++j)
    {
        if (j == 0)
        {
            fprintf(stdout, "|");
        }

        p = getClientAtPos(i);
        if (p != NULL)
        {
            s->map[i] = p->id;
        }
        else if (s->map[i] != CELL_WALL)
        {
            s->map[i] = CELL_EMPTY;
        }

        if (p != NULL)
        {
            fprintf(stdout, "%s|", p->name);
        }
        else{
            for (a = 0; a < 5; ++a)
            {
                fprintf(stdout, "%c", s->map[i]);
            }
            fprintf(stdout, "|");
        }

        if (j == size-1)
        {
            fprintf(stdout, "\n");
            j=-1;
        }
    }
}

void *Tick(){
    logger->dbg("Tick Launched");
    int i = 0;
    GameInfo* s = getServer();

    char buffer[100];
    while(s->game_status == 1){
        system("clear");
        sprintf(buffer, "Cycle: %d", i);

        logger->dbg(buffer);
        printMap();
        
        Publish(buffer);
        sleep(1);
        i++;
    }

    pthread_exit(NULL);
}

void* setMapSize(int size){
    logger->inf("    == Set Map Size (%d) ==", size);
    GameInfo* s = getServer();
    
    if (size > 21 || size < 5)
    {
        logger->err("     -Map %d not in Range (5, 21) Size set to Default (%d) ", size, 5);
        size = 5;
    }

    s->map_size = size;
    s->cells_cnt = size * size;
    logger->dbg("      mapsize: %d", s->cells_cnt);

    s->map = malloc(s->cells_cnt);

    if (s->map == NULL)
    {
        logger->err("      Fail to malloc");
        return NULL;
    }

    s->map[s->cells_cnt] = '\0';
}

void placeWalls(){
    logger->inf("    == Place Walls ==");
    int i;
    int obs;
    int pos;
    int size;
    int ratio;
    GameInfo* s = getServer();

    size = s->map_size;
    ratio = (size * 100) / 99;
    logger->dbg("      -Walls Ratio: %d", ratio);

    // obs = rand() % (ratio) -1;
    obs = ratio;

    for (i = 0; i < s->cells_cnt; ++i)
    {
        s->map[i] = CELL_EMPTY;
    }

    for (i = 0; i < obs; ++i)
    {
        pos = 0;
        while(pos == 0 || pos == size-1 || pos == size * (size-1) || pos == (size * size) -1){
            pos = rand() % ((size * size)- 1);
        }


        logger->dbg("      -Wall #%d: %d", i, pos);
        s->map[pos] = CELL_WALL;
    }

    logger->inf("    == Walls Done ==");
}


int initMap(){
    logger->inf("  == Init Map ==");
    GameInfo* s = getServer();
    if (s->map_size == 0){
        logger->dbg("    -Map size not Set Setting to default");
        setMapSize(5);
    }

    placeWalls();
    logger->inf("  == Map Done ==");
}

int createSockets(){
    GameInfo* s = getServer();
    s->sockets->context = zmq_ctx_new ();
    s->sockets->private = zmq_socket (s->sockets->context, ZMQ_REP);
    s->sockets->public = zmq_socket (s->sockets->context, ZMQ_PUB);

    int rc = zmq_bind (s->sockets->private, "tcp://*:5555");
    int pc = zmq_bind (s->sockets->public, "tcp://*:5566");

    assert(rc == 0);
    assert(pc == 0);

    return 1;
}

void initServArgs(){
    logger->dbg("- Init Server Args");
    GameInfo* srv = getServer();

    static Arg arg1 = {
        .name = "-size", 
        .function = setMapSize, 
        .hasParam = 1, 
        .defParam = NULL, 
        .asInt = 1, 
        .type="num"
    };

    static  Arg* params[] = {
        &arg1,
        NULL
    };

    srv->params = malloc(sizeof(ListManager));
    srv->params = defineArgs(params);
}

int main (int argc, char* argv[]){
    srand(time(0));
    int i = 0;

    logger = initLogger(argc, argv);
    GameInfo* s = initServer(argc, argv);

    if (s == NULL){
        logger->err("Error: Fail To init Server");
        return 1;
    }

    if (logger->lvl == DEBUG){
        logger->dbg("\n== Map Preview ==");
        printMap();
        logger->dbg("");
    }

    pthread_t handle;
    if (pthread_create(&handle, NULL, HandlePrivate, NULL)) {
        logger->err("pthread_create Tick");
        return EXIT_FAILURE;
    }
    logger->inf("\n== Private Thread Launched ==");


    int count = 0;
    logger->inf("== Waiting for Connections ==");
    while(count < 3){
        if (s->game_status){
            if (!count){
               logger->dbg("Game Ready");
            }

            logger->inf("Lauching in %dsec", 3-count);
            count++;
        }
        sleep(1);
    }

    pthread_t tick;
    if (pthread_create(&tick, NULL, Tick, NULL)) {
        logger->err("pthread_create Tick");

        return EXIT_FAILURE;
    }
    logger->dbg("Tick launched");

    if (pthread_join(tick, NULL)) {
        logger->err("Fail to Wait For Tick");
        return EXIT_FAILURE;
    }
    
    return 0;
}
