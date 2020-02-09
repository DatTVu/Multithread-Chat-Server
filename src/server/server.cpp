#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
int main(int argc, char* argv[]){
	struct addrinfo hints, *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];
	if(argc != 2){
		fprintf(stderr, "Usage: showip hostname\n");
		return 1;
	}
	memset(&hints, 0, sizeof(hints)); // make sure member of hints are set to 0
	hints.ai_family = AF_UNSPEC; //use either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;//use TCP sockets
	if((status=getaddrinfo(argv[1], NULL, &hints, &res))!=0){
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return 2;
	}
	
	printf("IP Addresses %s:\n\n", argv[1]);
	for(p = res; p!=NULL; p = p->ai_next){
		void *addr;
		char *ipver;
		//get the pointer to the address itself,
		//different fields in IPv4/IPv6
		if(p->ai_family == AF_INET){
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		
		}
		//convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
		printf(" %s: %s\n", ipver, ipstr);
	}
	freeaddrinfo(res);
	return 0;
}
