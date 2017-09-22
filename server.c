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

void genEnergy(){
    GameInfo* s = getServer();
    short pos = 0;
    short placed = 0;
    EnergyCell* cell;

    int co[2];
    do{
        pos = (rand() % (s->cells_cnt -2)) + 1;


        pos2coord(pos, co);

        if (getClientAtPos(pos)){
            continue;
        }

        if (s->map[pos] == CELL_ENRG){
            continue;
        }

        if (s->map[pos] == CELL_WALL){
            continue;
        }

        s->map[pos] = CELL_ENRG;
        
        cell->x = co[0];
        cell->y = co[1];
        cell->position = pos;

        cell->value = 5 + (rand() % 10);
        add_NodeV(s->energy_cells, "energy", (void*) cell);

        placed=1;
    }while(!placed);

    logger->dbg("Energy Placed At Pos: %d", pos);
}

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

    s->privPort = 0;
    s->pubPort = 0;
    
    initServArgs();
    parseArgs(s->params, argc, argv);

    if (!s->privPort){
        s->privPort = 4242;
    }

    if (!s->pubPort){
        s->pubPort = 4343;
    }

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

    logger->dbg("-Malloc Player Actions");
    s->player_actions = initListMgr();
    initPlayerArgs();
    if (s->player_actions == NULL){
        logger->err("Error: Faild to malloc Player Actions");
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

void* Respond(char* msg){
    GameInfo* s = getServer();
    zmq_send(s->sockets->private, msg, 100, 0);
}

void* Publish(char* msg){
    GameInfo* s = getServer();
    s_sendmore(s->sockets->public, "B");
    s_send(s->sockets->public, msg);
}

void* NewClent(char* name){
    GameInfo* s = getServer();
    if (s->players->nodeCount >= 4){
        Respond("ko");
        return NULL;
    }

    logger->dbg("adding client: %s", name);

    Player* p = malloc(sizeof(Player));
    p->name = name;

    if (getNodeByName(s->players, name) != NULL){
        logger->inf("Id already exists, refusing..");
        Respond("ko");
        return;
    }

    if (add_NodeV(s->players, name, p) == NULL){
        logger->err("Faild To Add New Player: %d", name);
        return;
    }

    int i;
    int pos;
    int vacant = 0;
    do{
        for (i = 0; i < 4; ++i){
            if (i > 1){
                pos = s->cells_cnt - ((i < 3) * (s->map_size)) -(i == 3);
            }
            else{
                pos = i * (s->map_size-1);
            }

            if (!getClientAtPos(pos)){
                vacant = 1;
                break;
            }
        }
    }while(!vacant);


    logger->dbg("MAP SIZE: %d", s->map_size);
    logger->dbg("pos: %d", pos);

    // if (pos == 0){
    //     p->position = 0;
    // }
    // else if(pos == 1){
    //     p->position = s->map_size -1;
    // }
    // else if(pos == 2){
    //     p->position = (s->map_size * s->map_size) - s->map_size;
    // }
    // else{
    //     p->position = (s->map_size * s->map_size) -1;
    // }

    p->position = pos;

    logger->dbg("New client at pos: %d", p->position);

    int co[2];
    pos2coord(p->position, co);
    logger->dbg("Coords: x:%d y:%d", co[0], co[1]);
    getClientAtPos(p->position);
    Respond("ok");

    if (s->players->nodeCount == 4){
        s->game_status = 1;
    }
}

int getPosition(Player* p){
    GameInfo* s = getServer();
    return p->x + (p->y * s->map_size);
}

int coord2pos(int x, int y){
    GameInfo* s = getServer();
    return x + (y * s->map_size);
}

void pos2coord(int pos, int res[]){
    GameInfo* s = getServer();
    res[1] = pos / s->map_size;
    res[0] = pos - (res[1] * s->map_size);
}

void* HandlePrivate(){
    char buffer[100];
    char* req[3];
    GameInfo* s = getServer();

    while(1){
        memset(buffer, 0, sizeof(buffer));
        zmq_recv(s->sockets->private, buffer, 100, 0);
        
        if (strlen(buffer) > 1){
            logger->dbg("Private => %s", buffer);
            memset(req, 0, sizeof(req));

            explode('|', buffer, 0, 4, req);
            logger->dbg("Action: %s", req[0]);
            logger->dbg("data: %s", req[1]);

            callArg(s->player_actions, req[0], req[1]);

            memset(buffer, 0, sizeof(buffer));
        }

        sleep(1);
    }
}


Player* getClientAtPos(int pos){
    GameInfo* s = getServer();
    Node* n = s->players->first;
    Player* p;

    if (n == NULL){
        return NULL;
    }

    int i=0;
    do{
        p = (Player*) n->value;

        if (p->position == pos){
            return p;
        }

        n = n->next;
    }while(n != s->players->first && n != NULL);

    return NULL;
}

void* printMap(){
    logger->dbg("printig");
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

        genEnergy();
        printMap();
        
        Publish(buffer);

        logger->dbg("Cycle time: %d", s->cycle);
        usleep(s->cycle);
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

void* setCycle(int c){
    if (c < 0){
        logger->war("Cylcle must be > 0\n Cycle set: to 1s");
        c = 1000;
    }

    GameInfo* s = getServer();
    s->cycle = c;
}

void* setPrivatePort(int p){
    if (p < 0){
        logger->war("Private port must be > 0\n port set: to 4242");
        p = 4242;
    }

    GameInfo* s = getServer();
    s->privPort = p;
}

void* setPublicPort(int p){
    if (p < 0){
        logger->war("Private port must be > 0\n port set: to 4343");
        p = 4343;
    }

    GameInfo* s = getServer();
    s->pubPort = p;
}

void* verbose(){
    setLogLvl(0);
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
    s->sockets->context = zmq_ctx_new();
    s->sockets->private = zmq_socket(s->sockets->context, ZMQ_REP);
    s->sockets->public = zmq_socket(s->sockets->context, ZMQ_PUB);
    char* url = malloc(sizeof(char) * 13);

    sprintf(url, "tcp://*:%d", s->privPort);    
    logger->inf("setting priv port to: %s", url);
    int rc = zmq_bind(s->sockets->private, url);

    sprintf(url, "tcp://*:%d", s->pubPort);    
    logger->inf("setting pub port to: %s", url);
    int pc = zmq_bind(s->sockets->public, "tcp://*:5566");

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

    static Arg arg2 = {
        .name = "-cycle", 
        .function = setCycle,
        .hasParam = 1, 
        .defParam = NULL, 
        .asInt = 1, 
        .type="num"
    };

    static Arg arg3 = {
        .name = "v", 
        .function = verbose,
        .hasParam = 0,
        .defParam = 0, 
        .asInt = 1, 
        .type="num"
    };

    static Arg arg4 = {
        .name = "-rep-port", 
        .function = setPrivatePort,
        .hasParam = 1,
        .defParam = NULL, 
        .asInt = 1, 
        .type="num"
    };

    static Arg arg5 = {
        .name = "-pub-port", 
        .function = setPublicPort,
        .hasParam = 1,
        .defParam = NULL, 
        .asInt = 1, 
        .type="num"
    };

    static  Arg* params[] = {
        &arg1,
        &arg2,
        &arg3,
        &arg4,
        &arg5,
        NULL
    };

    srv->params = malloc(sizeof(ListManager));
    srv->params = defineArgs(params);
}

void initPlayerArgs(){
    logger->dbg("- Init Player Args");
    GameInfo* srv = getServer();

    static Arg arg1 = {
        .name = "identify", 
        .function = NewClent, 
        .hasParam = 1, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static  Arg* player_actions[] = {
        &arg1,
        NULL
    };

    srv->player_actions = malloc(sizeof(ListManager));
    srv->player_actions = defineArgs(player_actions);
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
        logger->err("pthread_create Private");
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

        // printMap();
        sleep(1);
        // system("clear");
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
