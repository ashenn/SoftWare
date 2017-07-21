

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
#include "main.h"


// void *Tick(void *arg){
//     pthread_exit(NULL);
// }

void* Respond(GameInfo* s, char* msg){
    zmq_send(s->sockets->private, msg, 100, 0);
}

void* Publish(GameInfo* s, char* msg){
    s_sendmore(s->sockets->public, "B");
    s_send(s->sockets->public, msg);
}

int NewClent(GameInfo* s, char data[]){
    if (s->players->nodeCount >= 4){
        Respond(s, "ko");
        return 0;
    }

    printf("adding client:\n");
    int i;
    int id;
    int size;
    char* idstr;
    short added;
    Player* p = malloc(sizeof(Player));

    p->name = "#0x15";
    p->id = 1;
    p->position = 0;
    add_NodeV(s->players, p->name, (void*) p);

    return 1;
}

void* HandlePrivate(void* serv){
    printf("\n== handle Private ==\n");
    char buffer[100];
    GameInfo* s = (GameInfo*) serv;

    while(1){
        memset(buffer, 0, sizeof(buffer));
        zmq_recv(s->sockets->private, buffer, 10, 0);
        
        if (strlen(buffer) > 1){
            printf("Private => %s\n", buffer);

            if (!strcmp(buffer, "New Co")){
                printf("New Connection\n");
                NewClent(s, buffer);

                memset(buffer, 0, sizeof(buffer));
            }
        }

        sleep(1);
    }
}


Player* getClientAtPos(GameInfo* s,int pos){
    int i;
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


void* printMap(GameInfo* s){
    int a;
    int i;
    int j;
    int size;
    Player* p = NULL;

    size = s->map_size;

    j = 0;
    for (i = 0; i != s->cells_cnt; ++i, ++j)
    {
        if (j == 0)
        {
            printf("|");
        }

        p = getClientAtPos(s, i);
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
            printf("%s|", p->name);
        }
        else{
            for (a = 0; a < 5; ++a)
            {
                printf("%c", s->map[i]);
            }
            printf("|");
        }

        if (j == size-1)
        {
            printf("\n");
            j=-1;
        }
    }
}

void *Tick(void *game){
    printf("Tick Launched\n");
    int i = 0;
    GameInfo* s = (GameInfo*) game;

    char buffer[100];
    printf("TEST\n");
    while(1){
        system("clear");
        sprintf(buffer, "Cycle: %d", i);
        Publish(s, buffer);
        printMap(s);
        sleep(1);
        i++;
    }

    pthread_exit(NULL);
}

int setMapSize(GameInfo* s){
    int res;
    int size;

    res = 1;
    size = s->map_size;
    if (size > 21 || size < 5)
    {
        size = 10;
        res = 0;
    }

    s->map_size = size;
    s->cells_cnt = size * size;
    printf("mapsize: %d\n", s->cells_cnt);

    s->map = malloc(s->cells_cnt);

    if (s->map == NULL)
    {
        printf("Fail to malloc\n");
        return -1;
    }

    s->map[s->cells_cnt] = '\0';

    return res;
}

void placeWalls(GameInfo* s){
    int i;
    int obs;
    int pos;
    int size;
    int ratio;

    size = s->map_size;
    ratio = (size * 100) / 99;

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


        printf("placing Wall: %d\n", pos);
        s->map[pos] = CELL_WALL;
    }
}


int initMap(GameInfo* s){
    if(setMapSize(s) == -1){
        return -1;
    }

    placeWalls(s);
}



GameInfo* initServer(){
    static GameInfo* s = NULL;

    if (s != NULL)
    {
        return s;
    }

    s = malloc(sizeof(GameInfo));
    s->sockets = malloc(sizeof(Sockets));
    s->map_size = 5;
    s->game_status = 0;
    initMap(s);

    return s;
}

int createSockets(GameInfo* s){
    printf("Sockets INIT\n");
    s->sockets->context = zmq_ctx_new ();
    s->sockets->private = zmq_socket (s->sockets->context, ZMQ_REP);
    s->sockets->public = zmq_socket (s->sockets->context, ZMQ_PUB);

    int rc = zmq_bind (s->sockets->private, "tcp://*:5555");
    int pc = zmq_bind (s->sockets->public, "tcp://*:5566");

    assert(rc == 0);
    assert(pc == 0);

    return 1;
}

int main (void)
{

    srand(time(0));
    int i = 0;
    GameInfo* s = initServer();
    printMap(s);

    createSockets(s);
    printf("Sockets Done\n");


    pthread_t handle;
    if (pthread_create(&handle, NULL, HandlePrivate, s)) {
        perror("pthread_create Tick");
        return EXIT_FAILURE;
    }

    while(!s->game_status){
        printf("Waiting\n");
        sleep(1);
    }

    pthread_t tick;
    if (pthread_create(&tick, NULL, Tick, s)) {
        perror("pthread_create Tick");

        return EXIT_FAILURE;
    }

    printf("Thread launched\n");
    while (1) {
        sleep(1);          //  Do some 'work'
        // char buffer [10];
        // printf ("Received Hello\n");
        // printf ("Sending Private\n");
        // zmq_send (private, "World", 5, 0);
        // printf ("Done\n");

        // sprintf(buffer, "Public %d", i);
        // printf ("Sending Public %s\n", buffer);
        // s_sendmore (public, "B");
        // s_send (public, buffer);
        // printf ("Done\n");
        i++;
    }
    return 0;

    // pthread_t handlePrivate;
    // if (pthread_create(&handlePrivate, NULL, HandlePrivate, &sockets)) {
    //     perror("pthread_create Handle");

    //     return EXIT_FAILURE;
    // }

}
