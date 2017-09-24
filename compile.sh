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
	-llibzmq -lpthread

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
	/usr/local/lib/libzmq.so -lpthread

gcc -c \
	base/basic.c \
	base/libParse.c \
	base/logger.c \
	base/libList.c \
	common.c \
	client/client.c \
	-llibzmq -lpthread

gcc -o\
	cli\
	basic.o\
	libParse.o\
	logger.o\
	libList.o\
	common.o\
	client.o\
	/usr/local/lib/libzmq.so -lpthread

# gcc -c basic.c libParse.c logger.c libList.c client.c -llibzmq -lpthread
# gcc -o cli basic.o libParse.o logger.o libList.o client.o /usr/local/lib/libzmq.so -lpthread

rm *.o
# rm server/*.o