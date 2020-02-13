#ifndef MY_QUEUE_H
#define MY_QUEUE_H
#include <stdlib.h>
#include <WinSock2.h>
struct node {
	struct node* next;
	SOCKET *client_socket;
};
typedef struct node node_t;
void enqueue(SOCKET* client_socket);
SOCKET* dequeu();
#endif

