#include "main.h"
#include "server.h"


static void* showMap(void* p_data){
	server* s;

	s = (server*) p_data;

	while(1){
		printf("cycle: %d\n", s->cycle);
		printf("pos: %d\n", s->players[0].position);
		printf("nrg: %d\n", s->players[0].energy);
		printf("stun: %d\n", s->players[2].stun);
		printMap(s);
		usleep(100000);
		clearScreen();
	}
}

static void* initSocket(void* p_data){
	server* s;

	s = (server*) p_data;
	int i;
	int num;
	
	SOCKET sock;
	SOCKADDR_IN sin;
	SOCKET csock;
	SOCKADDR_IN csin;
	char buffer[32] = "";
	socklen_t recsize = sizeof(csin);
	int sock_err;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		// printf("invalid socket\n");
	}

	/* Configuration */
	sin.sin_addr.s_addr    = htonl(INADDR_ANY);   /* Adresse IP automatique */
	sin.sin_family         = AF_INET;             /* Protocole familial (IP) */
	sin.sin_port           = htons(PORT);         /* Listage du port */
	sock_err = bind(sock, (SOCKADDR*)&sin, sizeof(sin));

	if(sock_err == SOCKET_ERROR)
	{
		perror("bind");
	}

	sock_err = listen(sock, 5);
	// printf("Listage du port %d...\n", 365);

	if(sock_err == SOCKET_ERROR)
	{
		perror("listen");
	}

	for (i = 0; i < 4; ++i)
	{
		csock = accept(sock, (SOCKADDR*)&csin, &recsize);
		if(recv(csock, buffer, 32, 0) == SOCKET_ERROR){
			perror("recv");
		}

		// printf("len: %d\n", strLen(buffer));
		if (strComp(buffer, "new"))
		{
			num = addClient(s);
			if (num > -1)
			{
				printf("adding\n");
				if(send(csock, s->players[num].id, 32, 0) != SOCKET_ERROR)
				    printf("sended : %s\n", s->players[num].id);
				else
					printf("send error\n");
			}
			else{
				printf("fail to add\n");
			}
		}
		else{
			printf("not new\n");
		}
	}

	return NULL;
}

int main()
{
	int res;
	pthread_t screen;
	pthread_t socket;
	srand(time(0));
	server s;


	s.size = 12;
	if(!initServer(&s)){
		return 0;
	}

	res = pthread_create (&s.pid, NULL, startServer, &s);

	if (res)
	{
		fprintf (stderr, "%d", res);
		return 0;
	}

	res = pthread_create (&socket, NULL, initSocket, &s);

	if (res)
	{
		fprintf (stderr, "%d", res);
		return 0;
	}

	// res = pthread_create (
	// 	&screen, NULL,
	// 	showMap, (void *) &s
	// );

	// if (res)
	// {
	// 	fprintf (stderr, "%d", res);
	// 	return 0;
	// }

	pthread_join(s.pid, NULL);
	// pthread_cancel(screen);
	pthread_cancel(socket);
	return 0;
}