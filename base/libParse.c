#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>

#include "basic.h"
#include "libParse.h"


int verifyParam(char* p, char* type){
	if(!strcmp("alnum", type)){
		return alnum(p);
	}
	else if(!strcmp("alpha", type)){
		return strIsAlpha(p);
	}
	else if(!strcmp("num", type)){
		return strIsNum(p);
	}
	else{
		return 1;
	}
}

int callArg(ListManager* lstMgr, char* name, void* val){
	Arg* arg = NULL;
	// printB("Calling Arg");
	// printB(name);
	Node* node = getNodeByName(lstMgr, name);
	if (node == NULL){
		return 0;
	}

	arg = (Arg*) node->value;
	if(arg->hasParam){

		if (arg->asInt)
		{
			if (arg->defParam != NULL){
				arg->function(arg->defParam, (*(int*)val));
			}	
			else{
				arg->function((*(int*)val));
			}
		}
		else{
			if (arg->defParam != NULL){
				arg->function(arg->defParam, (char*)val);
			}
			else{
				arg->function((char*)val);
			}
		}
	}
	else{
		arg->function();
	}

	// printB("Calling Arg DONE");
	return 1;
}

int parseArgs(ListManager* lstMgr, int argc, char* argv[]){
	int i;
	int len;
	char* arg;
	Node* node = NULL;
	Arg* val = NULL;
	int v;

	Node* n = getNode(lstMgr, 1);
	for (i = 0; i < argc; ++i){
		if (i==0){
			continue;
		}

		len = strlen(argv[i]);	
		arg = malloc((len) * sizeof(char*));

		if (!isArgument(argv[i])){
			continue;		
		}

		substr(argv[i], 1, len, &arg);
		
		node = getNodeByName(lstMgr, arg);
		if (node == NULL)
		{
			free(arg);
			continue;
		}

		val = (Arg*) node->value;
		if(val->hasParam){
			if (isArgument(argv[i+1]))
			{
				free(arg);
				return -1;
			}

			if (val->asInt)
			{
				if (!strIsNum(argv[i+1]))
				{
					free(arg);
					return -1;
				}

				v = str2int(argv[++i]);

				if (val->defParam != NULL){
					val->function(val->defParam, &v);
				}
				else{
					val->function(v);
				}
			}
			else{
				if (!verifyParam(argv[i+1], val->type))
				{
					free(arg);
					return -1;
				}
				
				if (val->defParam != NULL){
					val->function(val->defParam, argv[i+1]);
				}
				else{
					val->function(argv[i+1]);
				}

				i++;
			}
		}
		else if (val->defParam != NULL){
			val->function(val->defParam);
		}
		else{
			val->function();
		}

		free(arg);
	}

	return 1;
}

int isArgument(char* str){
	return str[0] == '-';
}

ListManager* defineArgs(Arg** params){
	int i;
	Node* arg = NULL;
	ListManager* lstMgr = initLibParse();

	for (i=0; params[i] != NULL; ++i){
		arg = add_Node(lstMgr, params[i]->name);
		setValue(arg, params[i], 0);
	}

	return lstMgr;
}

ListManager* initLibParse(){
	ListManager* lstMgr = initListMgr(lstMgr);

	return lstMgr;
}
