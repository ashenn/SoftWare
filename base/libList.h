#ifndef __LIBLIST_H__
#define __LIBLIST_H__

typedef struct ListManager ListManager;
typedef struct Node Node;

struct Node{
	int id;
	char* name;
	ListManager* lstMgr;

	void* value;
	short valIsAlloc;

	Node* next;
	Node* prev;
};

struct ListManager
{
	int lastId;
	int nodeCount;

	Node* first;	
	Node* last;	
};

ListManager* initListMgr();
void* add_Node(ListManager* lstMgr, void* params);
void* add_NodeV(ListManager* lstMgr, void* params, void* value, short valueAlloc);
void printNode(Node* node);
void printNodes(ListManager* lstMgr);
Node* getNode(ListManager* lstMgr, int id);
Node* getNodeByName(ListManager* lstMgr, char* name);
void* del_Node(ListManager* lstMgr, int id);
char** getFunctionNames();
int setValue(Node* node, void* value, short asAlloc);
int* getIds(ListManager* lstMgr, int* ids);
void* clearList(ListManager* lstMgr);
void deleteList(ListManager* lstMgr);

#endif