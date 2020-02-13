#pragma once
#include <sys/types.h>
#include <sys/wait.h>

#define PORT "5000" // the port client will be connecting to
#define MAXDATASIZE 1024 // max number of bytes we can send at once
#define BACKLOG 10 //max number of thread server can create for thread pools
#define SOCKET_ERROR (-1) //most error with socket handling will return -1
#define THREAD_POOL_SIZE 10 //max number of thread for our thread pool

void *get_in_addr(struct sockaddr *sa){ //get the proper IPv4 or IPv6 IP
	if(sa->sa_family = AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}