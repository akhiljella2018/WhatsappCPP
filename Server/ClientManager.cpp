#include "ClientManager.h"
#include <algorithm>

void ClientManager::addClient(SOCKET socket)
{
    ClientInfo client;

    client.socket = socket;
    client.username = "";

    clients.push_back(client);
}

void ClientManager::removeClient(SOCKET socket)
{
    clients.erase(
        remove_if(
            clients.begin(),
            clients.end(),
            [socket](const ClientInfo& client)
            {
                return client.socket == socket;
            }),
        clients.end());
}

void ClientManager::setUsername(
    SOCKET socket,
    const string& username)
{
    for (auto& client : clients)
    {
        if (client.socket == socket)
        {
            client.username = username;
            return;
        }
    }
}

string ClientManager::getUsername(SOCKET socket) const
{
    for (const auto& client : clients)
    {
        if (client.socket == socket)
            return client.username;
    }

    return "";
}

vector<ClientInfo>& ClientManager::getClients()
{
    return clients;
}