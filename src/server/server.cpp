#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "../utilities/utils.h"
#include <pthread.h>
#include "../utilities/myqueue.h"
pthread_t thread_pool[THREAD_POOL_SIZE]; // creat a thread pools
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // init a mutex, this mutex is used later to makesure there is no race condition.
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER; // conditional variable to signal the thread in thread pools
void *handle_connection(void* client_socket); //a function to handle connection
void* thread_function(void *arg); //forward declaration an entry point of a p_thread.

int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p; 
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes=1;
	char s[INET6_ADDRSTRLEN]; //a char pointer to hold the IP infomation
	int rv;
	char buf[MAXDATASIZE]; // deprecated, used to hold a message that server got from client and send it back

	for (int i= 0; i < THREAD_POOL_SIZE; i++){ //init all the thread in thread pools and pass an entry point for each thread
		pthread_create(&thread_pool[i], NULL, &thread_function,NULL);
	}
	

	memset(&hints, 0, sizeof(hints)); //set every thing in hints equal to zero.
	hints.ai_family = AF_UNSPEC;// use either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; //use TCP protocol
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) { //fill the servinfo with internet host address and info to be used for bind() and connect()
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); //print out and error if there is any
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, //create a socket with the info from serveinfo
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) { // alow the socket to be binded
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) { // bind the socket
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // now p hold all the information we need, so we can free servinfo

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) { //start to listen on socket associated with file descriptor sockfd
		perror("listen");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		
		sin_size = sizeof(their_addr);
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size); //accept incoming connect
		if (new_fd == -1) {
		perror("accept");
		exit(1);
		}

		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof(s)); //convert value from network format to printable format
		printf("server: got connection from %s\n", s); // use the printable value to display client IP
		int *pclient = (int*)malloc(sizeof(int)); //we want to pass the client file descriptor as argument to the entry points of a thread. can't pass
		*pclient = new_fd; // an int as argument for p_thread. so allocate memory for an int pointer and point it to client file descriptor


		pthread_mutex_lock(&mutex); //since all threads in thread pool can be pushed into a same queue(effectively modify the queue), lock here before modify the queue to prevent data race
		enqueue(pclient); //now pass client file descriptor to a member in a queue. 
		pthread_cond_signal(&condition_var); //signal any available thread that there is now a member in the queue, so wake up grab that guy and start working
		pthread_mutex_unlock(&mutex);//done modifying the queue so unlock mutex
	}
	return 0;
}

void *handle_connection(void* p_client_socket){ 
	int client_socket = *(int*)p_client_socket; //grab the client file descriptor, deferrence it to a local variable
	free(p_client_socket); //client file descriptor is now stored locally, we don't need the pointer anymore, free it to avoid memory leak
	char buf[MAXDATASIZE];
	while(1){
		int result = recv(client_socket, buf, sizeof(buf), 0); //continously receive package from client
		if (result == -1 || result == 0){ // if there is an error then stop working, put the thread back to sleep
			perror("recv");
			break;
		}		
		buf[result] = '\0'; // null terminate the message received
		printf("%s", buf); // print out the message received
	}
	return NULL; // need to return a void pointer to match function return type when the function finishes its job
}

void * thread_function(void *arg){
	while(true){ // keep the thread alive
		int *pclient; //
		pthread_mutex_lock(&mutex); // going to modify the queue again so lock mutex
		if ((pclient = dequeue())==NULL){ // if there is nothing in the queue, wait for a signal from condition_var 
			pthread_cond_wait(&condition_var, &mutex); // there is a signal arrive, wake up and start working
			pclient = dequeue(); //we wake up, grab the guy in the queue so we need to remove that guy from the queue
		}
		pthread_mutex_unlock(&mutex);// done modify the queue so unlock mutx
		
		if (pclient != NULL){
			//we have a connection
			handle_connection(pclient); //handle the connection
		}
	}
}

