# SERVER
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

gcc -o\
	srv\
	basic.o\
	libParse.o\
	logger.o\
	libList.o\
	common.o\
	server.o\
	map.o\
	server_player.o\
	server_socket.o\
	/usr/local/lib/libzmq.so -lpthread -ljson

# VISU
gcc -c \
	base/basic.c \
	base/libParse.c \
	base/logger.c \
	base/libList.c \
	common.c \
	visualization/visu.c \
	-llibzmq -lpthread -ljson 

gcc -o\
	visu\
	basic.o\
	libParse.o\
	logger.o\
	libList.o\
	common.o\
	visu.o\
	/usr/local/lib/libzmq.so -lpthread -ljson

# CLIENT
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

gcc -o\
	cli\
	basic.o\
	libParse.o\
	logger.o\
	libList.o\
	common.o\
	identity.o \
	client.o \
	directions.o \
	handlers.o \
	actions.o \
	informations.o \
	ia.o \
	/usr/local/lib/libzmq.so -lpthread

# gcc -c basic.c libParse.c logger.c libList.c client.c -llibzmq -lpthread
# gcc -o cli basic.o libParse.o logger.o libList.o client.o /usr/local/lib/libzmq.so -lpthread

rm *.o
# rm server/*.o
