#ifndef MY_QUEUE_H_
#define MY_QUEUE_H_
#include <stdlib.h>
struct node{
    struct node* next;
    int *client_socket;
};
typedef struct node node_t;

node_t* head = NULL; // create a queue to make sure our thread pools always working
node_t* tail = NULL; // init our head and tail of the queue to NULL pointer
void enqueue(int* client_socket){
    node_t *newnode = (node_t*)malloc(sizeof(node_t)); // add a new node to the queue
    newnode->client_socket = client_socket; //update client descriptor value
    newnode->next = NULL; //there is no guy standing behind us in the queue
    if (tail == NULL){ // if there is nothing in the queue, set the head to new node
        head = newnode; 
    }
    else {
        tail->next = newnode; //else make the tail point to newnode to mark there is member in the queu
    }
    tail = newnode;//either way make tail point to new node to indicate there is member in the queue

}

int* dequeue(){
    if (head== NULL){ //head = NULL mean there is no work in queue so return NULL
        return NULL;
    }
    else{
        int* result = head->client_socket; // store the file of client fd in an int pointer
        node_t *temp = head; // create temp pointer point to head
        head = head->next; //update head to the next member in the queue
        if(head==NULL){tail=NULL;} //if there is no member in the queue, free temp
        free(temp);
        return result; //return client fd pointer to the thread so it can do work with it
    }
}
#endif //