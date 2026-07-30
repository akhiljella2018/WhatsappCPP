#pragma once

#include <winsock2.h>
#include <vector>
#include <string>

using namespace std;

struct ClientInfo
{
    SOCKET socket;
    string username;
};

class ClientManager
{
private:
    vector<ClientInfo> clients;

public:
    void addClient(SOCKET socket);

    void removeClient(SOCKET socket);

    void setUsername(SOCKET socket, const string& username);

    string getUsername(SOCKET socket) const;

    vector<ClientInfo>& getClients();
};