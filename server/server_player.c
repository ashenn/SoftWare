#include "server_player.h"


void* NewClent(char* name){
    GameInfo* s = getServer();
    logger->dbg("adding client: %s", name);

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

    s->map[pos] = CELL_PLAYER;

    logger->dbg("New client at pos: %d", p->position);
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