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

    s->cycle = 0;
    s->pubPort = 0;
    s->privPort = 0;
    
    initServArgs();
    parseArgs(s->params, argc, argv);

    if (!s->privPort){
        logger->war("Undefined Private port\nPort set to: %d", PORT_PUB);
        s->privPort = PORT_PRIV;
    }

    if (!s->pubPort){
        logger->war("Undefined Publish port\nPort set to: %d", PORT_PUB);
        s->pubPort = PORT_PUB;
    }

    if (!s->cycle){
        logger->war("Undefined cylcle time\nCycle set to: %d", CYCLE);
        s->cycle = CYCLE;
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

void* setCycle(int c){
    if (c < 0){
        logger->war("Cylcle must be > 0\n Cycle set: to 1s");
        c = 1000;
    }

    GameInfo* s = getServer();
    s->cycle = c;
}

void* verbose(){
    setLogLvl(0);
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
