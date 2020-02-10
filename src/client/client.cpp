#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../utilities/utils.h"

int main(int argc, char *argv[])
{
	int sockfd, result;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *serv_info, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	
	if(argc !=2){
		fprintf(stderr, "usage: client hostname\n");
		exit(1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((rv = getaddrinfo(argv[1], PORT, &hints, &serv_info)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	int retryCount = 3;
	// loop through all the results and connect to the first we can
	for(p = serv_info; p!=NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("client: socket");
			continue;
		}

		if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL){
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	
	
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
	printf("client: connecting to %s\n", s);

	freeaddrinfo(serv_info); // all done with this structure
	while(retryCount >0)
	{
		printf("\n\tEnter your message!");
		fgets(buf, sizeof(buf),stdin);
		if(strcmp(buf, "quit") == 0){
				printf("Ok! Quitting!");
				send(sockfd, buf, sizeof(buf), 0);
				retryCount = 0;
				}
		else{

			result = send(sockfd, buf, sizeof(buf), 0);
			if (result !=-1) continue;
			else{
				int time = 2000;
				printf("\n\tCannot connect to SERVER! Retrying in %d", time);
				--retryCount;
			}
		}

	}
	close(sockfd);
	return 0;
}











