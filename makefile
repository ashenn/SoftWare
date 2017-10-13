clean:
	rm -rf *.o

fclean: clean

compileServer: 
	gcc -o \
	srv \
	basic.o \
	libParse.o \
	logger.o \
	libList.o \
	common.o \
	server.o \
	map.o \
	server_player.o \
	server_socket.o \
	/usr/local/lib/libzmq.so -lpthread -ljson

serverO: 
	gcc -c \
	base/basic.c \
	base/libParse.c \
	base/logger.c \
	base/libList.c \
	common.c \
	server/server.c \
	server/map.c \
	server/server_player.c \
	server/server_socket.c \
	-llibzmq -lpthread -ljson 


server: serverO compileServer clean

compileClient: 
	gcc -o \
	cli \
	basic.o \
	libParse.o \
	logger.o \
	libList.o \
	common.o \
	identity.o \
	client.o \
	directions.o \
	handlers.o \
	actions.o \
	informations.o \
	ia.o \
	/usr/local/lib/libzmq.so -lpthread

clientO: 
	gcc -c \
	base/basic.c \
        base/libParse.c \
        base/logger.c \
        base/libList.c \
        common.c \
        client/actions/identity.c \
        client/actions/directions.c \
        client/actions/informations.c \
        client/actions/handlers.c \
        client/actions/actions.c \
        client/AI/ia.c \
        client/client.c \
        -llibzmq -lpthread

client: clientO compileClient clean

visuO: 
	gcc -c \
	base/basic.c \
	base/libParse.c \
	base/logger.c \
	base/libList.c \
	common.c \
	visualization/visu.c \
	-llibzmq -ljson 

compileVisu:	
	gcc -o \
	visu \
	basic.o \
	libParse.o \
	logger.o \
	libList.o \
	common.o \
	visu.o \
	/usr/local/lib/libzmq.so -ljson


visu: visuO compileVisu clean

all: server client visu

