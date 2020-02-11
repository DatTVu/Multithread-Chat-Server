# READ ME
1. About:
This is a simple multithreaded server demo. Included is source code for client and server. Multiple clients can run on localhost and connect simultaneously to a server hosted on localhost. The demo demonstrates how to use concepts such as
sockets, TCP/IP protocols, p_threads, mutex and conditional variables to program a multithreaded server.

2. Requirements:

[] Ubuntu 18.04

[] GCC 7.04

3. Folder structure:
src

/server/server.cpp

/client/client.cpp

/utilities/utils.h

/utilities/myqueue.h


README.md

4. To build:
+ Client: navigate to client directory and use the following command:
gcc -o client client.cpp
+ Server: navigate to server directory and use the following command:
gcc -pthread -o server server.cpp -lstdc++

5. To run:
+ Client: in a separate terminal, use the following command:
./client localhost
+ Server: in a separate terminal, use the following command:
./server
