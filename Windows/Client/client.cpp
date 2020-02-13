#pragma once
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#define SCK_VERSION2 0x0202
#define RETRY_TIME 5000

#define SERVERPORT 8989
#define BUFSIZE 1024
char myMessage[BUFSIZE];
using namespace std;
void main()
{
	long SUCCESSFUL;
	WSAData WinSockData;
	WORD DLLVersion;
	DLLVersion = MAKEWORD(2, 1);
	SUCCESSFUL = WSAStartup(DLLVersion, &WinSockData);

	string RESPONSE;
	string CONVERTER;
	SOCKADDR_IN ADDRESS;
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, NULL);

	ADDRESS.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_port = htons(8989);
		
	int retryCount = 3;
	connect(sock, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));

	while (retryCount > 0)
	{
		
		printf("\n\tEnter your message!");
		fgets(myMessage, sizeof(myMessage), stdin);
		SUCCESSFUL=send(sock, myMessage, sizeof(myMessage), 0);
			if (SUCCESSFUL != -1)
			{
				cout << "\n\tMessage from SERVER:\n\n\t" << CONVERTER << endl;
				continue;
			}
			else
			{
				cout << "\n\tCannot connect to SERVER!"<< endl;
				--retryCount;
			}
	}
	closesocket(sock);
	cout << "\n\n\t";
}