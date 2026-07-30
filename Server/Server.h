#pragma once

#include <winsock2.h>
#include <vector>
#include <mutex>
#include <string>
#include "../Common/Protocol.h"


using namespace std;

class Server
{
private:
    WSADATA wsa;
    SOCKET serverSocket;
    sockaddr_in serverAddress;
   struct ClientInfo
{
    SOCKET socket;
    string username;
};

vector<ClientInfo> clients;
vector<Message> chatHistory;
    mutex clientMutex;
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