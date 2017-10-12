#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libList.h"

ListManager* initListMgr(){	
	ListManager* lstMgr = malloc(sizeof(ListManager));
	if (lstMgr == NULL){
		return NULL;
	}

	lstMgr->lastId = 0;
	lstMgr->nodeCount = 0;
	lstMgr->first = NULL;
	lstMgr->last = NULL;

	return lstMgr;
}

void* add_Node(ListManager* lstMgr, void* params){
	int id;
	char* name = (char*) params;
	// printf("New Node Name %s\n", name);
	Node* newNode = malloc(sizeof(Node));
	if (newNode == NULL){
		printf("\n### Fail to alloc Node###\n");
		return NULL;
	}

	id = ++lstMgr->lastId;
	newNode->id = id;
	newNode->lstMgr = lstMgr;
	newNode->name = name;
	newNode->value = NULL;

	if (lstMgr->nodeCount == 0) // Is the first node
	{
		lstMgr->first = newNode;
		lstMgr->last = newNode;
	}
	else{
		newNode->prev = lstMgr->last;
		newNode->next = lstMgr->first;

		lstMgr->last->next = newNode;
		lstMgr->last = newNode;
		
		lstMgr->first->prev = newNode;
	}

	lstMgr->nodeCount++;
	return newNode;
}
void* add_NodeV(ListManager* lstMgr, void* params, void* value, short valueAlloc){
	Node* n = add_Node(lstMgr, params);
	if (n == NULL) {
		return NULL;
	}

	n->value = value;
	n->valIsAlloc = valueAlloc;
	return n;
}

void printNode(Node* node){
	if (node == NULL)
	{
		printf("node for printNode is NULL\n");
		return;
	}

	printf("name: %s \n", node->name);
	

	if (node->value == NULL)
	{
		printf("no Value Set\n");
	}
	else{
		printf("Value is Set\n");
	}

}

void printNodes(ListManager* lstMgr){
	Node* currentNode;
	currentNode = NULL;

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		printNode(currentNode);
	}
}

Node* getNode(ListManager* lstMgr, int id){
	Node* currentNode = NULL;

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		if (currentNode->id == id)
		{
			return currentNode;
		}
	}

	printf("## Error: Node with id * not found\n");
	return NULL;
}

Node* getNodeByName(ListManager* lstMgr, char* name){
	Node* currentNode = NULL;

	if (strlen(name) == 0)
	{
		printf("## Error: name param for getNode is invalid\n");
		return NULL;
	}

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		if (!strcmp(currentNode->name, name))
		{
			return currentNode;
		}
	}

	// printf("## Error: Node with name: %s not found", name);

	return NULL;
}



typedef struct del_node_param del_node_param;
struct del_node_param
{
	int id;
	char* name;
};

void* del_Node(ListManager* lstMgr, int id){
	Node* node = getNode(lstMgr, id);
	if (node == NULL){
		printf("\n## Error: Cant delete Node with id: *");
		return NULL;
	}

	Node* prev = node->prev;
	Node* next = node->next;

	if (prev != NULL && next != NULL){
		prev->next = next;
		next->prev = prev;
	}

	if (lstMgr->first == node){
		lstMgr->first = next;
		lstMgr->last->next = next;
	}

	if (lstMgr->last == node){
		lstMgr->last = prev;
	}

	if (node->valIsAlloc){
		free(node->value);
	}

	free(node);
	if (--lstMgr->nodeCount == 1){
		lstMgr->first->next = NULL;
		lstMgr->first->prev = NULL;
	}
	else if (lstMgr->nodeCount == 0){
		lstMgr->first = NULL;
		lstMgr->first = NULL;
	}

	return NULL;
}

char** getFunctionNames(){
	char** ptr_func = NULL;

	char* functions[] = {
		"addNode",
		"delNode"
	};
	
	ptr_func = functions;

	return ptr_func;
}

int setValue(Node* node, void* value, short asAlloc){
	if (node->value != NULL){
		free(node->value);
	}

	node->value = value;
	node->valIsAlloc = asAlloc;
	return 1;
}

int* getIds(ListManager* lstMgr, int* ids){
	if (lstMgr->nodeCount <= 0){
		return NULL;
	}

	int i=0;
	Node* currentNode = NULL;

	do{
		if (currentNode == NULL){
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		ids[i] = currentNode->id;
				
		i++;
	}while(lstMgr->last != currentNode);

	return ids;
}

void* clearList(ListManager* lstMgr){
	int i;
	int ids[lstMgr->nodeCount];

	getIds(lstMgr, ids);

	for (i = 0; lstMgr->nodeCount > 0; ++i){
		del_Node(lstMgr, ids[i]);
	}
}

void deleteList(ListManager* lstMgr){
	clearList(lstMgr);
	free(lstMgr);
}