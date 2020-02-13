#include "myqueue.h"
node_t *head = NULL; 
node_t *tail = NULL;

void enqueue(SOCKET* client_socket) {
	node_t *newnode = (node_t*)malloc(sizeof(node_t));
	newnode->client_socket = client_socket;
	newnode->next = NULL;
	if (tail == NULL) {
		tail = newnode;
	}
	else {
		tail->next = newnode;
	}
	tail = newnode;
}

SOCKET* dequeu() {
	if (head == NULL) {
		return NULL;
	}

	else {
		SOCKET* result = head->client_socket;
		node_t *temp = head;
		head = head->next;
		if (head == NULL) {
			tail = NULL;
		}
		free(temp);
		return result;
	}
}