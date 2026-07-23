#ifndef CLIENT_H
#define CLIENT_H
#pragma once
#include <winsock2.h>
#include <string>
using namespace std;

class Client
{
public:
    Client();
    ~Client();

    bool initialize();
    bool createSocket();
    bool connectToServer();
    void chat();
    void cleanup();

private:
    WSADATA wsa;

    SOCKET clientSocket;

    sockaddr_in serverAddress;
    string username;
};

#endif