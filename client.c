#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "zhelpers.h"

int main (void)
{
    printf ("Connecting to server…\n");
    void *context = zmq_ctx_new ();

    void *requester = zmq_socket (context, ZMQ_REQ);
    void *public = zmq_socket (context, ZMQ_SUB);

    zmq_connect (requester, "tcp://localhost:5555");
    zmq_connect (public, "tcp://localhost:5566");

    zmq_setsockopt (public, ZMQ_SUBSCRIBE, "B", 1);

    char buffer[100];
    // buffer = "";
    printf ("Sending Hello\n");
    sprintf(buffer, "%s", "identify|#0x29");
    printf("Sending %s\n", buffer);
    zmq_send (requester, buffer, 100, 0);
    memset(buffer, 0, sizeof(buffer));

    printf("Enter Loop\n");
    while(1){
        zmq_recv (requester, buffer, 100, 0);
        if (strlen(buffer) > 1)
        {
            printf ("Private %s\n", buffer);
            printf ("Len: %d\n", (int)strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
        }

        zmq_recv (public, buffer, 100, 0);
        if (strlen(buffer) > 1)
        {
            printf ("Public => %s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }

    }

    zmq_close (requester);
    zmq_close (public);
    zmq_ctx_destroy (context);
    return 0;
}