#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libList.h"

int main() {
	// Create List
	ListManager* list = initListMgr();

	//Add Empty Node
	Node* node_0 = add_Node(list, "node_0");
	printf("=== NODE 0 ==\n");
	printNode(node_0);
	printf("\n\n");


	int val_1 = 5;
	
	//Add Node With Value
	Node* node_1 = add_NodeV(list, "node_1", &val_1, 0);
	printf("=== NODE 1 ==\n");
	printNode(node_1);
	printf("\n\n");


	char* val_2 = malloc(12);
	strcpy(val_2, "Test String");

	//Add Node With Value
	add_NodeV(list, "node_2", val_2, 0);

	// Get Node
	Node* node_2 = getNode(list, 3);
	printf("=== NODE 2 ==\n");
	printNode(node_2);
	printf("Data: %s", (char*)(node_2->value));
	printf("\n\n");

	printf("=== Print List ==\n");
	printNodes(list);
	printf("\n\n");

	return 0;
}


