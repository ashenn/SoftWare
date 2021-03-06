#include <json/json.h>
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
    s->curPlayer = malloc(8);
    memset(s->curPlayer, 0, sizeof(s->curPlayer));
    
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
        closeServer();
        return NULL;
    }

    logger->dbg("-Malloc Players");
    s->players = initListMgr();
    if (s->players == NULL){
        logger->err("Error: Faild to malloc Players");
        closeServer();
        return NULL;
    }

    logger->dbg("-Malloc Player Actions");
    s->player_actions = initListMgr();
    initPlayerArgs();
    if (s->player_actions == NULL){
        logger->err("Error: Faild to malloc Player Actions");
        closeServer();
        return NULL;
    }

    logger->dbg("-Malloc Energy");
    s->energy_cells = initListMgr();
    if (s->energy_cells == NULL){
        logger->err("Error: Faild to malloc Energy");
        closeServer();
        return NULL;
    }


    initMap(s);

    logger->inf("== Server Done ==");
    return s;
}

GameInfo* getServer(){
    return initServer(0, NULL);
}

void closeServer(){
    GameInfo* s = getServer();
    deleteList(s->players);
    deleteList(s->params);
    deleteList(s->energy_cells);
    deleteList(s->player_actions);

    free(s->map);
    free(s->sockets);
}

void cell2json(json_object* array, EnergyCell* e){
    int coord[2];
    GameInfo* s = getServer();

    pos2coord(e->position, s->map_size, coord);
    json_object* cell = json_object_new_object();

    json_object_object_add(cell,"x", json_object_new_int(coord[0]));
    json_object_object_add(cell,"y", json_object_new_int(coord[1]));
    json_object_object_add(cell,"position", json_object_new_int(e->position));
    json_object_object_add(cell,"value", json_object_new_int(e->value));

    json_object_array_add(array, cell);
}

void cells2json(json_object* obj){
    Node* n;
    EnergyCell* e;
    GameInfo* s = getServer();

    json_object* cells = json_object_new_array();
    if (s->energy_cells->nodeCount){
        n = s->energy_cells->first;

        do{
            e = (EnergyCell*) n->value;
            cell2json(cells, e);
            n = n->next;

        }while(n != s->energy_cells->first && n != NULL);
    }

    json_object_object_add(obj, "energy_cells", cells);
}


void player2json(json_object* array, Player* p){
    int coord[2];
    GameInfo* s = getServer();

    pos2coord(p->position, s->map_size, coord);
    json_object* player = json_object_new_object();

    json_object_object_add(player,"x", json_object_new_int(coord[0]));
    json_object_object_add(player,"y", json_object_new_int(coord[1]));
    json_object_object_add(player,"position", json_object_new_int(p->position));
    json_object_object_add(player,"id", json_object_new_int(p->id));
    json_object_object_add(player,"name", json_object_new_string (p->name));
    json_object_object_add(player,"energy", json_object_new_int(p->energy));
    json_object_object_add(player,"action", json_object_new_int(p->action));
    json_object_object_add(player,"stuned", json_object_new_int(p->stuned));
    json_object_object_add(player,"looking", json_object_new_int(p->looking));

    json_object_array_add(array, player);
}

void players2json(json_object* obj){
    Node* n;
    Player* p;
    GameInfo* s = getServer();

    json_object* players = json_object_new_array();
    if (s->players->nodeCount){
        n = s->players->first;
        int i = 0;
        do{
            p = (Player*) n->value;
            player2json(players, p);
            n = n->next;

        }while(n != s->players->first && n != NULL);
    }

    json_object_object_add(obj, "players", players);
}

void walls2json(json_object* obj){
    GameInfo* s = getServer();
    json_object* walls = json_object_new_array();

    int i;
    for (i = 0; i < s->cells_cnt-1; ++i){
        if (s->map[i] == CELL_WALL){
            json_object_array_add(walls, json_object_new_int(i));
        }
    }

    json_object_object_add(obj, "walls", walls);
}

void notify(int type, json_object* data){
    json_object* cont = json_object_new_object();
    json_object_object_add(cont, "notification_type", json_object_new_int(type));    
    json_object_object_add(cont, "data", data);

    Publish("%s", json_object_to_json_string(cont));
}

void publishTick(){
    int z=0;
    int i=0;
    
    Player* p;
    GameInfo* s = getServer();


    json_object* obj = json_object_new_object();

    json_object_object_add(obj,"map_size", json_object_new_int(s->map_size));
    json_object_object_add(obj,"game_status", json_object_new_int(s->game_status));

    cells2json(obj);
    players2json(obj);
    walls2json(obj);
    
    notify(0, obj);
}

void* playerTickCheck(){
    int alive = 0;
    GameInfo* s = getServer();
    Node* n =  s->players->first;
    Player* p;

    do{
        p = (Player*) n->value;

        if (p->energy > 0){
            removeEnergy(p, 2);
        }

        if (p->energy > 0){
            alive++;
            p->action = 2;

            if (p->stuned){
                p->stuned--;
            }
        }

        n = n->next;
    }while(n != s->players->first && n != NULL);

    if (alive <= 0){
        s->game_status++;
    }
}

void reloadCells(){
    logger->dbg("Reloading cells");
    GameInfo* s = getServer();
    if (!s->energy_cells->nodeCount){
        logger->dbg("No Energy");
        return;
    }
    
    EnergyCell* e = NULL;
    Node* n = s->energy_cells->first;
    do{
        e = (EnergyCell*) n->value;
        
        s->map[e->position] = CELL_ENRG;
        n = n->next;
    }while(n != NULL && n != s->energy_cells->first);
}

void reloadPlayers(){
    logger->dbg("Reloading player");
    GameInfo* s = getServer();

    if (!s->players->nodeCount){
        logger->dbg("No Player");
        return;
    }

    Player* p = NULL;
    Node* n = s->players->first;
    do{
        p = (Player*) n->value;
        logger->dbg("Player: %s", p->name);

        if (p->energy > 0){
            logger->dbg("alive: %s | pos: %d", p->name, p->position);
            s->map[p->position] = CELL_PLAYER;
        }
        logger->dbg("Dead: %s", p->name);

        n = n->next;
    }while(n != NULL && n != s->players->first);
}

void reloadMap(){
    reloadCells();
    reloadPlayers();
}

void beforeTick(){
    logger->dbg("beforeTick");
    genEnergy();
    playerTickCheck();
    reloadMap();
}

void *Tick(){
    logger->dbg("Tick Launched");
    int i = 0;
    GameInfo* s = getServer();
    Node* n = NULL;
    Player* p;

    char buffer[100];
    while(s->game_status == 1){
        system("clear");
        sprintf(buffer, "Cycle: %d", i);
        logger->dbg(buffer);

        beforeTick();

        printMap();
        publishTick();
        
        // Publish(buffer);

        // logger->dbg("Cycle time: %d", s->cycle);
        usleep(s->cycle);
        i++;
    }

    logger->inf("Game FINISHED");
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

void* logFile(char* file){
    setLogFile(logger, file);
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

    static Arg arg6 = {
        .name = "-log",
        .function = logFile,
        .hasParam = 1,
        .defParam = NULL, 
        .asInt = 0,
        .type="any"
    };

    static  Arg* params[] = {
        &arg1,
        &arg2,
        &arg3,
        &arg4,
        &arg5,
        &arg6,
        NULL
    };

    srv->params = initListMgr();
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

        sleep(1);
    }

    notify(1, NULL);

    pthread_t tick;
    if (pthread_create(&tick, NULL, Tick, NULL)) {
        logger->err("pthread_create Tick");

        return EXIT_FAILURE;
    }

    if (pthread_join(tick, NULL)) {
        logger->err("Fail to Wait For Tick");
        return EXIT_FAILURE;
    }

    closeServer();
    
    return 0;
}
