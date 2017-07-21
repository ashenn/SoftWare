

//  Hello World server

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
#include "server.h"


GameInfo* initServer(int argc, char *argv[]){
    static GameInfo* s = NULL;

    if (s != NULL){
        return s;
    }

    fprintf(stdout, "== Init Server ==\n");

    s = malloc(sizeof(GameInfo));
    if (s == NULL){
        fprintf(stderr, "Error: Faild to malloc Server\n");
        return NULL;
    }

    fprintf(stdout, "-Setting params\n");
    s->map = NULL;
    s->map_size = 0;
    s->game_status = 0;
    
    initServArgs();
    parseArgs(s->params, argc, argv);


    fprintf(stdout, "-Malloc Sockets\n");
    s->sockets = malloc(sizeof(Sockets));
    createSockets();
    if (s->sockets == NULL){
        fprintf(stderr, "Error: Faild to malloc Sockets\n");
        free(s);
        return NULL;
    }

    fprintf(stdout, "-Malloc Players\n");
    s->players = initListMgr();
    if (s->players == NULL){
        fprintf(stderr, "Error: Faild to malloc Players\n");
        free(s);
        return NULL;
    }

    fprintf(stdout, "-Malloc Energy\n");
    s->energy_cells = initListMgr();
    if (s->energy_cells == NULL){
        fprintf(stderr, "Error: Faild to malloc Energy\n");
        free(s);
        return NULL;
    }


    initMap(s);

    fprintf(stdout, "== Server Done ==\n");
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

    fprintf(stdout, "adding client:\n");
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
            fprintf(stdout, "Private => %s\n", buffer);

            if (!strcmp(buffer, "New Co")){
                fprintf(stdout, "New Connection\n");
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
    fprintf(stdout, "Tick Launched\n");
    int i = 0;
    GameInfo* s = getServer();

    char buffer[100];
    fprintf(stdout, "TEST\n");
    while(s->game_status == 1){
        system("clear");
        sprintf(buffer, "Cycle: %d", i);

        Publish(buffer);
        printMap();
        sleep(1);
        i++;
    }

    pthread_exit(NULL);
}

void* setMapSize(int size){
    fprintf(stdout, "    == Set Map Size (%d) ==\n", size);
    GameInfo* s = getServer();
    
    if (size > 21 || size < 5)
    {
        fprintf(stdout, "     -Map %d not in Range (5, 21) Size set to Default (%d) \n", size, 5);
        size = 5;
    }

    s->map_size = size;
    s->cells_cnt = size * size;
    fprintf(stdout, "      mapsize: %d\n", s->cells_cnt);

    s->map = malloc(s->cells_cnt);

    if (s->map == NULL)
    {
        fprintf(stdout, "      Fail to malloc\n");
        return NULL;
    }

    s->map[s->cells_cnt] = '\0';
}

void placeWalls(){
    fprintf(stdout, "    == Place Walls ==\n");
    int i;
    int obs;
    int pos;
    int size;
    int ratio;
    GameInfo* s = getServer();

    size = s->map_size;
    ratio = (size * 100) / 99;
    fprintf(stdout, "      -Walls Ratio: %d\n", ratio);

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


        fprintf(stdout, "      -Wall #%d: %d\n", i, pos);
        s->map[pos] = CELL_WALL;
    }

    fprintf(stdout, "    == Walls Done ==\n");
}


int initMap(){
    fprintf(stdout, "  == Init Map ==\n");
    GameInfo* s = getServer();
    if (s->map_size == 0){
        fprintf(stdout, "    -Map size not Set Setting to default\n");
        setMapSize(5);
    }

    placeWalls();
    fprintf(stdout, "  == Map Done ==\n");
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
    fprintf(stdout, "- Init Server Args\n");
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
    GameInfo* s = initServer(argc, argv);

    if (s == NULL){
        fprintf(stderr, "Error: Fail To init Server\n");
        return 1;
    }

    fprintf(stdout, "\n== Map Preview ==\n\n");
    printMap();
    fprintf(stdout, "\n");

    pthread_t handle;
    if (pthread_create(&handle, NULL, HandlePrivate, NULL)) {
        perror("pthread_create Tick");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "\n== Private Thread Launched ==\n");


    int count = 0;
    fprintf(stdout, "== Waiting for Connections ==\n");
    while(count < 3){
        if (s->game_status){
            if (!count){
                fprintf(stdout, "Game Ready\n");
            }

            fprintf(stdout, "Lauching in %dsec\n", 3-count);
            count++;
        }
        sleep(1);
    }

    pthread_t tick;
    if (pthread_create(&tick, NULL, Tick, NULL)) {
        perror("pthread_create Tick");

        return EXIT_FAILURE;
    }

    fprintf(stdout, "Thread launched\n");
    while (1) {
        sleep(1);
        i++;
    }
    
    return 0;
}
