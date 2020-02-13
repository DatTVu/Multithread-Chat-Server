#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include "myqueue.h"
#define SCK_VERSION2 0x0202 //Winsock version 2
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 

#define SERVERPORT 8989
#define BUFSIZE 1024
#define SERVER_BACKLOG 10
#define THREAD_POOL_SIZE 2
using std::cout;
using std::cin;
using std::endl;

struct message {
	int id_ = 0;
	char message_[BUFSIZE];
};

struct message myMassage_;

std::thread thread_pool_[THREAD_POOL_SIZE];
std::mutex mutex_;
std::condition_variable condition_var_;

void handle_connection(SOCKET *client_socket);
void* thread_function();
int main()
{
    //local variables;;
    long SUCCESSFUL;
    WSAData WinSockData;
    WORD DLLVERSION;

    DLLVERSION = MAKEWORD(2, 1); //macro to create WORD value by concatenating its arguments

    //Start Winsock DLL
    SUCCESSFUL = WSAStartup(DLLVERSION, &WinSockData);

	for (int i = 0; i < THREAD_POOL_SIZE; i++) {
		thread_pool_[i] = std::thread(thread_function);
	}
    //Create Socket Structure
	SOCKADDR_IN ADDRESS;
    int AddressSize = sizeof(ADDRESS);

    //Create Sockets
    SOCKET sock_Listen; //listen for incoming connection
    SOCKET sock_Connection = INVALID_SOCKET; //activate if connection found

	sock_Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_Listen == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
    sock_Connection = socket(AF_INET, SOCK_STREAM, 0);
    ADDRESS.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //Set IP
    ADDRESS.sin_family = AF_INET;
    ADDRESS.sin_port = htons(8989);
	
	if (::bind(sock_Listen,(SOCKADDR *)& ADDRESS, AddressSize) == SOCKET_ERROR) {
		wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
		closesocket(sock_Listen);
		WSACleanup();
		return 1;
	}
	listen(sock_Listen, THREAD_POOL_SIZE);
	cout << "\n\tServer: Waiting for incoming connections...";
    for(;;)
    {
		sock_Connection = accept(sock_Listen, (SOCKADDR*)&ADDRESS, &AddressSize);
		//sock_Connection = accept(sock_Listen, (SOCKADDR*)&ADDRESS, &AddressSize);
		if (sock_Connection == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(sock_Listen);
			WSACleanup();
			return 1;
		}

		SOCKET* sockfd = (SOCKET*)malloc(sizeof(SOCKET));
		*sockfd = sock_Connection;
		std::unique_lock<std::mutex> lock_(mutex_);
		enqueue(sockfd);
		condition_var_.notify_one();
    }
}

void handle_connection(SOCKET *client_socket) {
	char buffer[BUFSIZE];
	SOCKET clientfd = *client_socket;
	free(client_socket);
	while (true) {
		int result;
		result = recv(clientfd, buffer, sizeof(buffer), NULL);
		if (result == -1 || result == 0) { // if there is an error then stop working, put the thread back to sleep
			perror("recv");
			break;
		}
		send(clientfd, buffer, sizeof(buffer), NULL);
	}
}

void* thread_function() {
	while (true) {
		SOCKET *pclient_sock;
		std::unique_lock<std::mutex> lock_(mutex_);
		if ((pclient_sock = dequeu()) == NULL)
		{
			condition_var_.wait(lock_);
			pclient_sock = dequeu();
		}
		lock_.unlock();
		if (pclient_sock != NULL) {
			handle_connection(pclient_sock);
		}
	}
}