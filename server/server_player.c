#include "server_player.h"
short takeAction(Player* p ,int amt){
    if (p->action < amt){
        Respond("ko|Not enougth action.");
        return 0;
    }

    p->action -= amt;
    return 1;
}

Player* getCurPlayer(){
    GameInfo* s = getServer();
    Node* n = getNodeByName(s->players, s->curPlayer);

    if (n == NULL){
        return NULL;
    }

    return (Player*) n->value;
}

void* NewClent(char* name){
    GameInfo* s = getServer();
    logger->dbg("adding client: %s", name);
    if (s->game_status > 0){
        logger->inf("Game Started Refusing.");
        Respond("ko|game started");
        return;
    }

    if (s->players->nodeCount >= 4){
    	logger->dbg("Max client Reached refusing.");
        Respond("ko");
        return NULL;
    }

    if (getNodeByName(s->players, name) != NULL){
        logger->inf("Id already exists, refusing.");
        Respond("ko");
        return;
    }

    int i;
    int pos;
    int look;
    int vacant = 0;
    for (i = 0; i < 4; ++i){
        if (i > 1){
            look = UP;
            pos = s->cells_cnt - ((i < 3) * (s->map_size)) -(i == 3);
        }
        else{
            look = DOWN;
            pos = i * (s->map_size-1);
        }

        if (!getClientAtPos(pos)){
            vacant = 1;
            break;
        }
    }

    if (!vacant){
        logger->err("Faild To Add New Player: No vacant place left");
        Respond("ko");
        return;
    }

    int co[2];
    pos2coord(pos, s->map_size, co);

    Player* p = malloc(sizeof(Player));
    p->x = co[0];
    p->y = co[1];
    
    p->name = name;
    p->looking = look;
    p->position = pos;

    p->action = 2;
    p->energy = 50;

    s->map[pos] = CELL_PLAYER;

    logger->dbg("New client '%s' at pos: %d", p->name, p->position);
    logger->dbg("look: %s", getLookName(look));
    logger->dbg("Coords: x:%d y:%d", p->x, p->y);


    Node* n = add_NodeV(s->players, name, p);
    if (n == NULL){
        logger->err("Faild To Add New Player Node");
        Respond("ko");
        free(p);
        return;
    }

    Respond("ok");
        s->game_status = 1;
    if (s->players->nodeCount == 4){
    }
}

int getPosition(Player* p){
    GameInfo* s = getServer();
    return coord2pos(p->x, p->y, s->map_size);
}

Player* getClientAtPos(int pos){
    GameInfo* s = getServer();
    Node* n = s->players->first;
    Player* p;

    if (n == NULL){
        return NULL;
    }

    do{
        p = (Player*) n->value;

        if (p->position == pos){
            return p;
        }

        n = n->next;
    }while(n != s->players->first && n != NULL);

    return NULL;
}

void rotate(Player* p, int dir){
    p->looking += dir; 
    if (p->looking > 3){
        p->looking = 0;
    }
    else if (p->looking < 0){
        p->looking = 3;
    }
}

void* right(){
    Player* p = getCurPlayer();
    if (!takeAction(p, 1)){
        return;
    }

    rotate(p, TURN_RIGHT);
    Respond("ok");
}

void* left(){
    Player* p = getCurPlayer();
    if (!takeAction(p, 1)){
        return;
    }

    rotate(p, TURN_LEFT);
    Respond("ok");
}

void* selfid(){
    GameInfo* s = getServer();
    Respond(s->curPlayer);
}

void* selfstats(){
    Player* p = getCurPlayer();
    char m[4];
    sprintf(m, "%d", p->energy);
    Respond(m);
}

void* removeEnergy(Player* p, int amt){
    // logger->inf("Process: '%s' lost energy: %d", p->name, amt);
    // return;
    p->energy -= amt;
    // logger->inf("Energy Left: %d", p->energy);

    if (p->energy > 0){
        return;
    }
    p->energy = 0;
    
    char m[25];
    sprintf(m, "Process: '%s' is dead.", p->name);
    logger->inf("Process: '%s' is dead.", p->name);
    Publish(m);
    assert(1);
}


void* gatherEnergy(){
    Player* p = getCurPlayer();
    if (!takeAction(p, 2)){
        return;
    }

    EnergyCell* e = getEnergyAtPos(p->position);
    if (e == NULL){
        Respond("ok|No energy");
    }
    GameInfo* s = getServer();

    p->energy += e->value;
    del_Node(s->energy_cells, e->id);
    s->map[p->position] = CELL_PLAYER;
}

void* HandlePrivate(){
    char buffer[100];
    char* req[4];
    GameInfo* s = getServer();

    while(1){
        memset(buffer, 0, sizeof(buffer));
        zmq_recv(s->sockets->private, buffer, 100, 0);
        
        if (strlen(buffer) > 1){
            logger->dbg("Private2 => %s", buffer);
            // rc = zmq_getsockopt(s->sockets->private, ZMQ_IDENTITY, &test, &size);
            
            memset(req, 0, sizeof(req));
            logger->dbg("exlode");
            explode('|', buffer, 0, 5, req);

            logger->dbg("Id: %s", req[0]);
            logger->dbg("Action: %s", req[1]);
            logger->dbg("data: %s", req[2]);

            strcpy(s->curPlayer, req[0]);
            callArg(s->player_actions, req[1], req[2]);

            memset(buffer, 0, sizeof(buffer));
        }

        sleep(1);
    }
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
    srv->player_actions = defineArgs(player_actions);
}
