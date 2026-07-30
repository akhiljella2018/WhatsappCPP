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
        cout << "Server received type = "
     << static_cast<int>(msg.type)
     << endl;
        cout << "RAW Packet : " << buffer << endl;
        cout << "Type : " << static_cast<int>(msg.type) << endl;

        // LOGIN Packet
if (msg.type == MessageType::LOGIN)
{
    {
        lock_guard<mutex> lock(clientMutex);

        for (auto& client : clients)
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

    // Send previous chat history only to this client
    sendChatHistory(clientSocket);

    // Notify everyone else
    Message joinMsg;

    joinMsg.type = MessageType::CHAT;
    joinMsg.sender = "SERVER";
    joinMsg.receiver = "";
    joinMsg.timestamp = getCurrentTimestamp();
    joinMsg.text = msg.sender + " joined the chat.";

    broadcast(
        serialize(joinMsg),
        clientSocket
    );

    continue;
}

        // CHAT Packet
else if (msg.type == MessageType::CHAT)
{
    cout << msg.sender
         << " : "
         << msg.text
         << endl;

    // Save message into chat history
    {
        lock_guard<mutex> lock(clientMutex);

        chatHistory.push_back(msg);

        // Keep only the latest 50 messages
        if (chatHistory.size() > 50)
        {
            chatHistory.erase(chatHistory.begin());
        }
    }

    // Broadcast to all other clients
    broadcast(
        serialize(msg),
        clientSocket
    );
}

        // EXIT Packet
        else if (msg.type == MessageType::PRIVATE_MESSAGE)
{
    cout << "[PRIVATE] "
         << msg.sender
         << " -> "
         << msg.receiver
         << " : "
         << msg.text
         << endl;

    sendPrivateMessage(msg);

    continue;
}

else if (msg.type == MessageType::USERS)
{
    sendOnlineUsers(clientSocket);
    continue;
}

else if (msg.type == MessageType::EXIT)
{
    cout << "[EXIT] "
         << msg.sender
         << " left the chat."
         << endl;

    Message exitMsg;

    exitMsg.type = MessageType::CHAT;
    exitMsg.sender = "SERVER";
    exitMsg.receiver = "";
    exitMsg.text = msg.sender + " left the chat.";

    broadcast(
        serialize(exitMsg),
        clientSocket
    );

    break;
}
    }

    // Remove client from list
    // Save username before removing client
string disconnectedUser;

{
    lock_guard<mutex> lock(clientMutex);

    for (const auto& client : clients)
    {
        if (client.socket == clientSocket)
        {
            disconnectedUser = client.username;
            break;
        }
    }

    clients.erase(
        remove_if(
            clients.begin(),
            clients.end(),
            [clientSocket](const ClientInfo& client)
            {
                return client.socket == clientSocket;
            }),
        clients.end());
}

// Notify everyone
if (!disconnectedUser.empty())
{
    Message leaveMsg;

    leaveMsg.type = MessageType::CHAT;
    leaveMsg.sender = "SERVER";
    leaveMsg.receiver = "";
    leaveMsg.timestamp = getCurrentTimestamp();
    leaveMsg.text = disconnectedUser + " left the chat.";

    broadcast(
        serialize(leaveMsg),
        INVALID_SOCKET
    );
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

void Server::sendPrivateMessage(const Message& msg)
{
    lock_guard<mutex> lock(clientMutex);

    string packet = serialize(msg);

    bool found = false;

    for (const auto& client : clients)
    {
        if (client.username == msg.receiver)
        {
            send(
                client.socket,
                packet.c_str(),
                static_cast<int>(packet.length()),
                0
            );

            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "[SERVER] User "
             << msg.receiver
             << " not online."
             << endl;
    }
}

void Server::sendOnlineUsers(SOCKET clientSocket)
{
    lock_guard<mutex> lock(clientMutex);

    Message reply;

    reply.type = MessageType::USERS;
    reply.sender = "SERVER";
    reply.receiver = "";

    reply.text = "===== ONLINE USERS ===== : ";

bool first = true;

for (const auto& client : clients)
{
    if (!client.username.empty())
    {
        if (!first)
            reply.text += ", ";

        reply.text += client.username;
        first = false;
    }
}

    string packet = serialize(reply);

    send(
        clientSocket,
        packet.c_str(),
        static_cast<int>(packet.length()),
        0
    );
}

void Server::sendChatHistory(SOCKET clientSocket)
{
    lock_guard<mutex> lock(clientMutex);

    // Send heading
    Message heading;

    heading.type = MessageType::CHAT;
    heading.sender = "SERVER";
    heading.receiver = "";
    heading.timestamp = getCurrentTimestamp();
    heading.text = "========== CHAT HISTORY ==========";

    string packet = serialize(heading);

    send(
        clientSocket,
        packet.c_str(),
        static_cast<int>(packet.length()),
        0
    );

    // Send every stored message
    for (const auto& msg : chatHistory)
    {
        string packet = serialize(msg);

        send(
            clientSocket,
            packet.c_str(),
            static_cast<int>(packet.length()),
            0
        );
    }

    // Send ending
    Message footer;

    footer.type = MessageType::CHAT;
    footer.sender = "SERVER";
    footer.receiver = "";
    footer.timestamp = getCurrentTimestamp();
    footer.text = "==================================";

    packet = serialize(footer);

    send(
        clientSocket,
        packet.c_str(),
        static_cast<int>(packet.length()),
        0
    );
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