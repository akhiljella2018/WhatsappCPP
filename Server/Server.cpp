#include "Server.h"
#include "../Common/Protocol.h"
#include <iostream>
#include <thread>
#include <algorithm>

using namespace std;


// Constructor
Server::Server()
{
    serverSocket = INVALID_SOCKET;
}


// Destructor
Server::~Server()
{
    cleanup();
}


// Initialize Server
bool Server::initialize()
{
    cout << "==================================" << endl;
    cout << "     WhatsApp Server Starting     " << endl;
    cout << "==================================" << endl;


    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        cout << "WSAStartup failed!" << endl;
        return false;
    }


    cout << "[OK] Winsock initialized." << endl;


    if(!createSocket())
        return false;


    if(!bindSocket())
        return false;


    if(!startListening())
        return false;


    return true;
}



// Create Socket
bool Server::createSocket()
{
    serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );


    if(serverSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed!" << endl;

        WSACleanup();

        return false;
    }


    cout << "[OK] Socket created." << endl;

    return true;
}



// Bind Socket
bool Server::bindSocket()
{

    serverAddress.sin_family = AF_INET;

    serverAddress.sin_port = htons(8080);

    serverAddress.sin_addr.s_addr = INADDR_ANY;



    if(bind(
        serverSocket,
        (sockaddr*)&serverAddress,
        sizeof(serverAddress)
    ) == SOCKET_ERROR)
    {

        cout << "Bind failed Error : "
             << WSAGetLastError()
             << endl;


        closesocket(serverSocket);

        WSACleanup();

        return false;
    }



    cout << "[OK] Socket bound to Port 8080."
         << endl;


    return true;
}



// Listen
bool Server::startListening()
{

    if(listen(
        serverSocket,
        SOMAXCONN
    ) == SOCKET_ERROR)
    {

        cout << "Listen failed!"
             << endl;


        return false;
    }


    cout << "[OK] Listening..."
         << endl;


    return true;
}



// Accept multiple clients
void Server::chat()
{

    while(true)
    {

        cout << "Waiting for client..."
             << endl;



        sockaddr_in clientAddress;

        int clientSize =
            sizeof(clientAddress);



        SOCKET newClientSocket =
            accept(
                serverSocket,
                (sockaddr*)&clientAddress,
                &clientSize
            );



        if(newClientSocket == INVALID_SOCKET)
        {

            cout << "Accept failed!"
                 << endl;

            continue;
        }



        cout << "[OK] Client Connected!"
             << endl;



        {
            lock_guard<mutex> lock(clientMutex);

            //clients.push_back(newClientSocket);
            ClientInfo client;

            client.socket = newClientSocket;
            client.username = "";

            clients.push_back(client);
        }



        thread clientThread(
            &Server::handleClient,
            this,
            newClientSocket
        );


        clientThread.detach();

    }

}





// Handle individual client
void Server::handleClient(SOCKET clientSocket)
{
    char buffer[1024];

    cout << "Client handler started..." << endl;

    while (true)
    {
        ZeroMemory(buffer, sizeof(buffer));

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer),
            0
        );

        if (bytesReceived <= 0)
        {
            cout << "Client disconnected." << endl;
            break;
        }

        // Convert received packet into Message object
        Message msg = deserialize(buffer);

        // LOGIN Packet
        if (msg.type == MessageType::LOGIN)
        {
            {
                lock_guard<mutex> lock(clientMutex);

                for (auto &client : clients)
                {
                    if (client.socket == clientSocket)
                    {
                        client.username = msg.sender;
                        break;
                    }
                }
            }

            cout << "[LOGIN] "
                 << msg.sender
                 << " joined the chat."
                 << endl;

            broadcast(
                msg.sender + " joined the chat.",
                clientSocket
            );
        }

        // CHAT Packet
        else if (msg.type == MessageType::CHAT)
        {
            cout << msg.sender
                 << " : "
                 << msg.text
                 << endl;

            broadcast(
                msg.sender + " : " + msg.text,
                clientSocket
            );
        }

        // EXIT Packet
        else if (msg.type == MessageType::EXIT)
        {
            cout << msg.sender
                 << " left the chat."
                 << endl;

            broadcast(
                msg.sender + " left the chat.",
                clientSocket
            );

            break;
        }
    }

    // Remove client from list
    {
        lock_guard<mutex> lock(clientMutex);

        clients.erase(
            remove_if(
                clients.begin(),
                clients.end(),
                [clientSocket](const ClientInfo &client)
                {
                    return client.socket == clientSocket;
                }),
            clients.end());
    }

    closesocket(clientSocket);
}

// Broadcast message
void Server::broadcast(
    string message,
    SOCKET sender
)
{
    lock_guard<mutex> lock(clientMutex);

    for (const auto& client : clients)
    {
        if (client.socket != sender)
        {
            send(
                client.socket,
                message.c_str(),
                static_cast<int>(message.length()),
                0
            );
        }
    }
}




// Cleanup
void Server::cleanup()
{

    if(serverSocket != INVALID_SOCKET)
    {

        closesocket(serverSocket);

        serverSocket = INVALID_SOCKET;
    }



    WSACleanup();


    cout << "Server shutdown completed."
         << endl;

}