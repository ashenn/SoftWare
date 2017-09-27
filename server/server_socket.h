#include "server.h"
int createSockets();
void* Respond(char* msg, ...);
void* Publish(char* msg);
void* setPublicPort(int p);
void* setPrivatePort(int p);