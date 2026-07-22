#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <string>

class Server
{
public:
    Server();
    ~Server();

    bool initialize();
    bool createSocket();
    bool bindSocket();
    bool startListening();
    bool acceptClient();
    void chat();
    void handleClient(SOCKET clientSocket);
    void cleanup();

private:
    WSADATA wsa;

    SOCKET serverSocket;
    SOCKET clientSocket;

    sockaddr_in serverAddress;
    sockaddr_in clientAddress;

    int clientSize;
};

#endif