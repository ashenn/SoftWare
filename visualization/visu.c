#include <json-c/json.h>
#include "visu.h"

Visu* initVisu(){
	static Visu* v;

	if (v != NULL){
		return v;
	}

	v = malloc(sizeof(Visu));
	v->sockets = malloc(sizeof(Sockets));

	v->size = 0;
	v->status = 0;
	v->sockets->context = zmq_ctx_new();
	v->sockets->public = zmq_socket (v->sockets->context, ZMQ_SUB);

	zmq_connect (v->sockets->public, "tcp://localhost:4343");
	zmq_setsockopt(v->sockets->public, ZMQ_SUBSCRIBE, "B", 1);

	v->players = initListMgr();
	v->cells = initListMgr();
	v->walls = initListMgr();

	return v;
}

Visu* getVisu(){
	return initVisu();
}

Player* getPlayerAtPos(int pos){
	Player* p = NULL;
	Visu* v = getVisu();
	Node* n = v->players->first;

	if (n == NULL){
	    return NULL;
	}

	do{
	    p = (Player*) n->value;

	    if (p->position == pos){
	        return p;
	    }

	    n = n->next;
	}while(n != v->players->first && n != NULL);

	return NULL;
}

int getEnergyAtPos(int pos){
	Visu* v = getVisu();
	Node* n = v->cells->first;

	if (n == NULL){
	    return 0;
	}

	do{
	    if (n->id == pos){
	        return 1;
	    }

	    n = n->next;
	}while(n != v->cells->first && n != NULL);

	return 0;
}

int getWallAtPos(int pos){
	Visu* v = getVisu();
	Node* n = v->walls->first;

	if (n == NULL){
	    return 0;
	}

	do{
	    if (n->id == pos){
	        return 1;
	    }

	    n = n->next;
	}while(n != v->walls->first && n != NULL);

	return 0;
}

void printPlayer(Player* p, int y, int pi){
	// logger->dbg("Print Player %d => %d", y, pi);
	// logger->dbg("TEST0");
	int x;
	char line[10];
	// logger->dbg("TEST0.1");
	
	// logger->dbg("TEST0.2 => %s", p->name);
	char* name = p->name;
	int look = p->looking;
	// logger->dbg("TEST0.3");
	memset(line, 0, sizeof(line));

	// logger->dbg("TEST0.4");
	if (!y){
		if (look == UP){
			fprintf(stdout, "  +++  |");
			// logger->dbg("Print End");
			return;
		}

		fprintf(stdout, "       |");
		// logger->dbg("Print End");
		return;
	}

	// logger->dbg("TEST1");
	if (y == 2){
		if (look == DOWN){
			fprintf(stdout, "__+++__|");
			// logger->dbg("Print End");
			return;
		}

		fprintf(stdout, "_______|");
		// logger->dbg("Print End");
		return;
	}

	// logger->dbg("TEST2");
	for (x = 0; x < 7; ++x){
		// logger->dbg("--x:%d", x);
		if (!x){
			// logger->dbg("TEST2.1");
			fprintf(stdout, (look == LEFT) ? " " : "+");
			continue;
		}
		
		if (x==1 && y ==1){
			// logger->dbg("TEST2.2");
			fprintf(stdout, "%s", name);
			x += 5;
		}

		if (x == 6){
			// logger->dbg("TEST2.3");
			fprintf(stdout, (look == RIGHT) ? " " : "+");
		}
	}	
	
	fprintf(stdout, "|");
	// logger->dbg("Print End");
}

void printEnergy(int y){
	if (!y){
		printf("   @   |");
	}
	else if(y==1){
		printf(" @@@@@ |");
	}
	else{
		printf("___@___|");
	}
}

void printWall(int y){
	if (!y){
		printf("  ###  |");
	}
	else if(y==1){
		printf("#######|");
	}
	else{
		printf("__###__|");
	}
}

void printEmpty(int y){
	if (!y){
		printf("       |");
	}
	else if(y==1){
		printf("       |");
	}
	else{
		printf("_______|");
	}
}


void printMap(){
	// logger->dbg("### Print Map ##");
	int i, x, y, z, pi, pos;
	i = x = y = z = pi = pos = 0;
	Player* p[5];
	Visu* v = getVisu();
	char line[v->size+1];
	
	for (y = 0; y < v->size; ++y){
		for (i = 0; i < 4; ++i){
			p[i] = NULL;
		}

		// logger->dbg("y: %d", y);
		if (!y){
			for (z = 0; z < v->size; ++z){
				printf("________");
			}
			printf("\n");
		}

		// logger->dbg("Memset");
		memset(line, 0, sizeof(line));
		for (x = pi = 0; x < v->size; ++x){
			// logger->dbg("x: %d", x);
			pos = coord2pos(x,y,v->size);

			p[pi] = getPlayerAtPos(pos);
			if (p[pi] != NULL){
				// logger->dbg("player");
				pi++;
				line[x] = CELL_PLAYER;
				continue;
				// printPlayer(p);
			}
			
			if (getEnergyAtPos(pos)){
				// logger->dbg("energy");
				line[x] = CELL_ENRG;
				// printEnergy();
				continue;
			}

			if (getWallAtPos(pos)){
				// logger->dbg("Wall");
				line[x] = CELL_WALL;
				// printWall();
				continue;
			}
			
			// logger->dbg("Empty");
			line[x] = CELL_EMPTY;
		}

		// logger->dbg("printing");
		for (z = 0; z < 3; ++z){
			pi = 0;
			// logger->dbg("z: %d", z);
			for (x = 0; x < v->size; ++x){
			// logger->dbg("x: %d", x);
				switch(line[x]){
					case CELL_EMPTY:
						printEmpty(z);
						break;

					case CELL_PLAYER:
						// logger->dbg("pi: %d", pi);
						printPlayer(p[pi], z, pi);
						pi++;
						break;

					case CELL_WALL:
						printWall(z);
						break;

					case CELL_ENRG:
						printEnergy(z);
						break;
				}
			}
			
			fprintf(stdout, "\n");
		}

		fprintf(stdout, "\n");
	}

	fprintf(stdout, "\n");
}

void initArgs(){
	Visu* v = getVisu();

	// static Arg arg1 = {
	//     .name = "-size", 
	//     .function = setMapSize, 
	//     .hasParam = 1, 
	//     .defParam = NULL, 
	//     .asInt = 1, 
	//     .type="num"
	// };


	// static  Arg* params[] = {
	//     &arg1,
	//     &arg2,
	//     &arg3,
	//     &arg4,
	//     &arg5,
	//     &arg6,
	//     NULL
	// };

	// srv->params = initListMgr();
	// srv->params = defineArgs(params);
}

/*printing the value corresponding to boolean, double, integer and strings*/
void print_json_value(json_object *jobj){
  enum json_type type;
  printf("type: %d\n",type);
  type = json_object_get_type(jobj); /*Getting the type of the json object*/
  switch (type) {
    case json_type_boolean: printf("json_type_boolean\n");
                         printf("value: %s\n", json_object_get_boolean(jobj)? "true": "false");
                         break;
    case json_type_double: printf("json_type_double");
                        printf("          value: %lf\n", json_object_get_double(jobj));
                         break;
    case json_type_int: printf("json_type_intn");
                        printf("          value: %d\n", json_object_get_int(jobj));
                         break;
    case json_type_string: printf("json_type_string\n");
                         printf("          value: %s\n", json_object_get_string(jobj));
                         break;
  }

}

void json_parse_array( json_object *jobj, char *key) {
  void json_parse(json_object * jobj); /*Forward Declaration*/
  enum json_type type;

  json_object *jarray = jobj; /*Simply get the array*/
  if(key) {
     json_object_object_get_ex(jobj, key, &jarray); /*Getting the array if it is a key value pair*/
  }

  int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
  printf("Array Length: %d\n",arraylen);
  int i;
  json_object * jvalue;

  for (i=0; i< arraylen; i++){
    jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
    if (type == json_type_array) {
      json_parse_array(jvalue, NULL);
    }
    else if (type != json_type_object) {
      printf("value[%d]: \n",i);
      print_json_value(jvalue);
    }
    else {
      json_parse(jvalue);
    }
  }
}

void storePlayers(json_object* obj){
	logger->dbg("Storing Players");
	Visu* v = getVisu();
	clearList(v->players);

	int i=0;
	int len=0;
	Node* n = NULL;
	Player* p = NULL;

	json_object *tmp = NULL;
	json_object *jarray = obj;
	json_object_object_get_ex(obj, "players", &jarray);

	len = json_object_array_length(jarray);

	json_object* player;
	for (i = 0; i < len; ++i){
		p = malloc(sizeof(Player));
		player = json_object_array_get_idx(jarray, i);

		json_object_object_get_ex(player, "name", &tmp);

		// n = add_Node(v->players, tmp);
		n = add_NodeV(v->players, p->name, p, 1);
		if (n == NULL){
			logger->err("Fail To add Player Node");
			continue;
		}
		n->valIsAlloc=1;

		p->name = malloc(8);
		logger->dbg("New Player: %s", json_object_get_string(tmp));
		strcpy(p->name, json_object_get_string(tmp));
		// logger->dbg("New Player: %s", p->name);
		free(tmp);


		json_object_object_get_ex(player, "energy", &tmp);

		// logger->dbg("-eng: %d", json_object_get_int(tmp));
		p->energy = json_object_get_int(tmp);
		logger->dbg("-eng: %d", p->energy);
		free(tmp);

		json_object_object_get_ex(player, "action", &tmp);

		// logger->dbg("-act: %d", json_object_get_int(tmp));
		p->action = json_object_get_int(tmp);
		logger->dbg("-act: %d", p->action);
		free(tmp);

		json_object_object_get_ex(player, "looking", &tmp);

		// logger->dbg("-look: %s", getLookName(json_object_get_int(tmp)));
		p->looking = json_object_get_int(tmp);
		logger->dbg("-look: %d", p->looking);
		free(tmp);

		json_object_object_get_ex(player, "stuned", &tmp);

		// logger->dbg("-stun: %d", json_object_get_int(tmp));
		p->stuned = json_object_get_int(tmp);
		logger->dbg("-stun: %d", p->stuned);
		free(tmp);

		json_object_object_get_ex(player, "position", &tmp);

		// logger->dbg("-pos: %d", json_object_get_int(tmp));
		p->position = json_object_get_int(tmp);
		logger->dbg("-pos: %d", p->position);
		free(tmp);

		// n->id = json_object_get_int(tmp);
		// logger->dbg("Player Added at pos: %d", n->id);
		logger->dbg("Player Added");
	}

}

void storeCells(json_object* obj){
	logger->dbg("Storing Cells");
	Visu* v = getVisu();
	clearList(v->cells);

	int i=0;
	int len=0;
	Node* n = NULL;

	json_object *posJ = NULL;
	json_object *jarray = obj;
	json_object_object_get_ex(obj, "energy_cells", &jarray);

	len = json_object_array_length(jarray);

	json_object* cell;
	for (i = 0; i < len; ++i){

		cell = json_object_array_get_idx(jarray, i);
		json_object_object_get_ex(cell, "position", &posJ);

		n = add_Node(v->cells, "cell");

		if (n != NULL){
			n->id = json_object_get_int(posJ);
		}
	}

}

void storeWalls(json_object* obj){
	logger->dbg("Storing Walls");
	Visu* v = getVisu();
	clearList(v->walls);

	int i=0;
	int len=0;
	Node* n = NULL;

	json_object *posJ = NULL;
	json_object *jarray = obj;
	json_object_object_get_ex(obj, "walls", &jarray);

	len = json_object_array_length(jarray);

	json_object* wall;
	for (i = 0; i < len; ++i){

		wall = json_object_array_get_idx(jarray, i);
		// json_object_object_get_ex(wall, "position", &posJ);
		n = add_Node(v->walls, "wall");


		if (n != NULL){
			n->id = json_object_get_int(wall);
			logger->dbg("Wall added at: %d", n->id);
		}
	}

}

void storeData(json_object* data){
	logger->dbg("### Storing Data");
	Visu* v = getVisu();
	enum json_type type;

	json_object_object_foreach(data, key, val) {
		logger->dbg("--Parsing: %s", key);
		type = json_object_get_type(val);

		if (!strcmp(key, "map_size")){
			v->size = json_object_get_int(val);
		}
		else if (!strcmp(key, "game_status")){
			v->status = json_object_get_int(val);
		}
		else if (!strcmp(key, "energy_cells")){
			storeCells(data);
		}
		else if (!strcmp(key, "walls")){
			if (!v->walls->nodeCount){
				storeWalls(data);
			}
		}
		else if (!strcmp(key, "players")){
			storePlayers(data);
		}
	}
}

void handle(int notif, json_object* data){
	static int i = 0;
	logger->dbg("====== Handeling notif [%d] ======", i++);

	switch (notif){
		case 0:
			logger->inf("#### New Cycle ####");
			storeData(data);
			break;
		case 1:
			logger->inf("#### Game Started ####");
			break;

		case 2:
			logger->inf("#### Game Ended ####");
			break;

		case 3:
			logger->inf("#### A player is dead ####");
			break;

		case 4:
			logger->inf("#### A player haw WON ####");
			break;
	}
	logger->dbg("====== Notif Done [%d] ======", i);
}

/*Parsing the json object*/
void json_parse(json_object * jobj) {
	logger->dbg("## Parsing Request##");
	enum json_type type;
	int notifType;
	json_object* data = NULL;

	json_object_object_foreach(jobj, key, val) { /*Passing through every array element*/
		logger->dbg("-- key: %s", key);
		type = json_object_get_type(val);

	    switch (type) {
	      	case json_type_int: 
	      		notifType = json_object_get_int(val);
	           break; 

	      	case json_type_object: 
	      		data = val;
	           break; 
		}

	}

	handle(notifType, data);
} 


int main(int argc, char const *argv[])
{
	srand(time(0));

	logger = initLogger(argc, argv);
	Visu* v = getVisu();
	json_object* jobj = NULL;

	char buffer[5000];

	int i=0;
	logger->inf("Stating Program");
	while(v->status < 2){
		memset(buffer, 0, sizeof(buffer));

		zmq_recv (v->sockets->public, buffer, 5000, 0);
		
		if (strlen(buffer) <= 1){
			continue;
		}

		logger->dbg("====== Notify ======\n %s", buffer);
		
		logger->dbg("--tokener");
		jobj = json_tokener_parse(buffer);
		
		logger->dbg("--parse");
		json_parse(jobj);
		
		logger->dbg("--free");
		free(jobj);

		printMap();
	}

	return 0;
}
