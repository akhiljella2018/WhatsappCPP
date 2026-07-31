#pragma once

#include <winsock2.h>
#include <vector>
#include <mutex>
#include <string>
#include "../Common/Protocol.h"
#include "ChatHistory.h"
#include "ClientManager.h"
#include "Authentication.h"
#include "Database.h"


using namespace std;

class Server
{
private:
    WSADATA wsa;
    SOCKET serverSocket;
    sockaddr_in serverAddress;
    ClientManager clientManager;
    //vector<Message> chatHistory;
    ChatHistory chatHistory;
    Authentication authentication;
    Database database;
    mutex clientMutex;

    void processLogin(
    SOCKET clientSocket,
    const Message& msg
);

void processRegister(
    SOCKET clientSocket,
    const Message& msg
);

void processChat(
    SOCKET clientSocket,
    const Message& msg
);

void processPrivateMessage(
    const Message& msg
);

void processUsers(
    SOCKET clientSocket
);

void processExit(
    SOCKET clientSocket,
    const Message& msg
);
public:

    Server();
    ~Server();
    bool initialize();
    void chat();
    void handleClient(SOCKET clientSocket);
    void broadcast(string message, SOCKET sender);
    void sendPrivateMessage(const Message& msg);
    void sendOnlineUsers(SOCKET clientSocket);
    void sendChatHistory(SOCKET clientSocket);
    void cleanup();

private:

    bool createSocket();
    bool bindSocket();
    bool startListening();

};