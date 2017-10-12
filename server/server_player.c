#include "server_player.h"
short takeAction(Player* p ,int amt){
    if (p->action < amt){
        Respond("ko|Not enougth action. (%d / %d)", p->action, amt);
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

Player* getPlayerByName(char* name){
    GameInfo* s = getServer();
    Node* n = s->players->first;
    Player* p;

    if (n == NULL){
        return NULL;
    }

    do{
        p = (Player*) n->value;

        if (strcmp(p->name, name)){
            return p;
        }

        n = n->next;
    }while(n != s->players->first && n != NULL);

    return NULL;
}

void* NewClent(char* name){
    GameInfo* s = getServer();
    logger->dbg("adding client: %s", name);
    if (s->game_status > 0){
        logger->inf("Game Started Refusing.");
        Respond("ko|game started");
        return NULL;
    }

    if (s->players->nodeCount >= 4){
    	logger->dbg("Max client Reached refusing.");
        Respond("ko|Game full");
        return NULL;
    }

    if (getNodeByName(s->players, name) != NULL){
        logger->inf("Id already exists, refusing.");
        Respond("ko|identity already exists");
        return NULL;
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
        Respond("ko|No position available");
        return NULL;
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
    p->stuned = 0;
    p->energy = 50;

    s->map[pos] = CELL_PLAYER;

    logger->dbg("New client '%s' at pos: %d", p->name, p->position);
    logger->dbg("look: %s", getLookName(look));
    logger->dbg("Coords: x:%d y:%d", p->x, p->y);


    Node* n = add_NodeV(s->players, name, p, 1);
    if (n == NULL){
        logger->err("Faild To Add New Player Node");
        Respond("ko|List manager Error");
        free(p);
        return NULL;
    }

    Respond("ok|%d;%d", pos, s->map_size);
    if (s->players->nodeCount == 4){
        s->game_status = 1;
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
        Respond("ko|Not enougth energy");
        return NULL;
    }

    rotate(p, TURN_RIGHT);
    Respond("ok");
}

void* left(){
    Player* p = getCurPlayer();
    if (!takeAction(p, 1)){
        return NULL;
    }

    rotate(p, TURN_LEFT);
    Respond("ok");
}

void* selfid(){
    GameInfo* s = getServer();
    Respond("ok|%s", s->curPlayer);
}

void* removeEnergy(Player* p, int amt){
    p->energy -= amt;

    if (p->energy > 0){
        return NULL;
    }
    p->energy = 0;
    
    logger->err("3|Process: '%s' is dead.", p->name);
    Publish("3|Process: '%s' is dead.", p->name);
}


void* gatherEnergy(){
    Player* p = getCurPlayer();
    if (!takeAction(p, 2)){
        return NULL;
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
    Player* p;

    while(s->game_status < 2){
        p = NULL;
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
            p = getCurPlayer();
            
            if (!strlen(req[2])){
                req[2] = NULL;
            }

            if (p != NULL && p->stuned){
                Respond("ko|Stuned");
            }
            else if (!callArg(s->player_actions, req[1], req[2])){
                Respond("ko|Bad action");
            }

            memset(buffer, 0, sizeof(buffer));
        }
    }

    int i;
    for (i = 0; i < 3; ++i){
        free(req[i]);
    }
}

void* vision(){
    Player* p = getCurPlayer();
    
    char* vision = malloc(25);
    // memset(vision, 0, sizeof(vision));
    getVison(p, vision);
    // logger->err("VISION: %s", vision);

    Respond("ok|%s", vision);
    free(vision);
}

void* look(){
    Player* p = getCurPlayer();
    if (p == NULL){
        Respond("ko");
    }

    Respond("ok|%d", p->looking);
}

void* selfstats(){
    Player* p = getCurPlayer();
    if (p == NULL){
        Respond("ko");
    }

    Respond("ok|%d", p->energy);
}

void* inspect(char* name){
    Player* p = getCurPlayer();
    if (!takeAction(p, 1)){
        Respond("ko|Not enougth energy");
        return NULL;
    }

    p = getPlayerByName(name);
    logger->dbg("Searching for: %s", name);
    if (p == NULL){
        Respond("ko|Bad ID");
    }

    Respond("ok|%d", p->energy);
}

short move(Player* p, int pos, int dir, int cost){
    GameInfo* s = getServer();
    logger->inf("Moving to: %d", pos);
    if (!posInBound(pos, s->map_size, dir)){
        logger->war("Cell %d out of bound", pos);
        return 0;
    }

    if (s->map[pos] != CELL_EMPTY && s->map[pos] != CELL_ENRG){
        logger->war("Cell %d Occupided: %c", pos, s->map[pos]);
        return -1;
    }


    if (!takeAction(p, cost)){
        return 2;
    }

    s->map[pos] = CELL_PLAYER;
    s->map[p->position] = CELL_EMPTY;

    p->position = pos;
    return 1;
}

void* forward(int mult){
    logger->inf("Forward: %d", mult);
    Player* p = getCurPlayer();

    GameInfo* s = getServer();

    int i;
    int pos = p->position;

    if (p->looking % 2){
        i = 1 * (-1 * (p->looking == RIGHT));
    }
    else{
        i = s->map_size * (1 - (2 * (p->looking == UP)));
    }

    pos += i * mult;

    i = move(p, pos, p->looking, 1 + (mult < 0));
    
    if (!i){
        Respond("ko|Out of bound");
        return NULL;
    }

    if (i < 0){
        Respond("ko|Cell Occupided");
        return NULL;
    }

    if (i > 1){
        return NULL;
    }

    Respond("ok");
}

void* backward(){
    forward(-1);
}

void* jump(){
    GameInfo* s = getServer();
    Player* p = getCurPlayer();

    int step;
    if (p->looking % 2){
        step = 2;
    }
    else{
        step = s->map_size *2;
    }

    if (p->looking == UP || p->looking == LEFT){
        step *= -1;
    }

    int pos = p->position + step;
    logger->inf("Juping From: %d to %d", p->position, pos);
    step = move(p, pos, p->looking, 2);

    if (!step){
        Respond("ko|Out of bound");
        return NULL;
    }

    if (step < 0){
        Respond("ko|Cell Occupided");
        return NULL;
    }

    if (step > 1){
        return NULL;
    }

    Respond("ok");
}

void* attack(){
    logger->inf("Attacking");

    GameInfo* s = getServer();
    Player* p = getCurPlayer();

    if(!takeAction(p, 1)){
        return NULL;
    }

    p->energy--;

    char line[10];
    getLine(p->position, p->looking, 4, 0, line, 1);
    logger->inf("line: %d | %s", charCnt(';', line, 0), line);
    if (!strlen(line)){
        return NULL;
    }

    int step;
    if (p->looking % 2){
        step = 1;
    }
    else{
        step = s->map_size;
    }

    if (p->looking == UP || p->looking == LEFT){
        step *= -1;
    }

    int i;
    Player* p2 = NULL;
    for (i = 0; i <= charCnt(';', line, 0); ++i){
        logger->inf("%d => pos: %d",i, p->position + step + (step *i));
        
        p2 = getClientAtPos(p->position + step + (step *i));
        if (p2 == NULL){
            continue;
        }

        p2->stuned=2;
        Publish("Player: %s is stuned !", p->name);
    }
}

void* rightfwd(){
    GameInfo* s = getServer();
    Player* p = getCurPlayer();

    int pos = p->position;
    switch(p->looking){
        case UP:
            pos += 1;
            break;

        case DOWN:
            pos -= 1;
            break;

        case RIGHT:
            pos += s->map_size;
            break;

        case LEFT:
            pos -= s->map_size;
            break;
    }

    pos = move(p, pos, RIGHT, 2);

    if (!pos){
        Respond("ko|Out of bound");
        return NULL;
    }

    if (pos < 0){
        Respond("ko|Cell Occupided");
        return NULL;
    }

    if (pos > 1){
        return NULL;
    }

    Respond("ok");
}

void* leftfwd(){
    GameInfo* s = getServer();
    Player* p = getCurPlayer();

    int pos = p->position;
    switch(p->looking){
        case UP:
            pos -= 1;
            break;

        case DOWN:
            pos += 1;
            break;

        case RIGHT:
            pos -= s->map_size;
            break;

        case LEFT:
            pos += s->map_size;
            break;
    }

    pos = move(p, pos, LEFT, 2);

    if (!pos){
        Respond("ko|Out of bound");
        return NULL;
    }

    if (pos < 0){
        Respond("ko|Cell Occupided");
        return NULL;
    }

    if (pos > 1){
        return NULL;
    }

    Respond("ok");
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
        .function = left, 
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

    static Arg arg5 = {
        .name = "watch", 
        .function = vision, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg6 = {
        .name = "looking", 
        .function = look, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg7 = {
        .name = "selfstats", 
        .function = selfstats, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg8 = {
        .name = "inspect", 
        .function = inspect, 
        .hasParam = 1, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg9 = {
        .name = "forward", 
        .function = forward, 
        .hasParam = 1, 
        .defParam = "1", 
        .asInt = 1, 
        .type="any"
    };

    static Arg arg10 = {
        .name = "gather", 
        .function = gatherEnergy, 
        .hasParam = 1, 
        .defParam = "1", 
        .asInt = 1, 
        .type="any"
    };

    static Arg arg11 = {
        .name = "backward", 
        .function = backward, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg12 = {
        .name = "jump", 
        .function = jump, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg13 = {
        .name = "attack", 
        .function = attack, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg14 = {
        .name = "rightfwd", 
        .function = rightfwd, 
        .hasParam = 0, 
        .defParam = NULL, 
        .asInt = 0, 
        .type="any"
    };

    static Arg arg15 = {
        .name = "leftfwd", 
        .function = leftfwd, 
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
        &arg5,
        &arg6,
        &arg7,
        &arg8,
        &arg9,
        &arg10,
        &arg11,
        &arg12,
        &arg13,
        &arg14,
        &arg15,
        NULL
    };

    srv->player_actions = initListMgr();
    srv->player_actions = defineArgs(player_actions);
}