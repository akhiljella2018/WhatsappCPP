#include "Server.h"

#include <iostream>

using namespace std;

// Constructor
Server::Server()
{
    serverSocket = INVALID_SOCKET;
    clientSocket = INVALID_SOCKET;
    clientSize = sizeof(clientAddress);
}

// Destructor
Server::~Server()
{
    cleanup();
}

// Initialize Winsock and start the server
bool Server::initialize()
{
    cout << "==================================" << endl;
    cout << "     WhatsApp Server Starting     " << endl;
    cout << "==================================" << endl;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "WSAStartup failed!" << endl;
        return false;
    }

    cout << "[OK] Winsock initialized." << endl;

    if (!createSocket())
        return false;

    if (!bindSocket())
        return false;

    if (!startListening())
        return false;

    if (!acceptClient())
        return false;

    return true;
}

// Create Socket
bool Server::createSocket()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
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

    if (bind(serverSocket,
             (sockaddr*)&serverAddress,
             sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cout << "Bind failed! Error Code: "
             << WSAGetLastError() << endl;

        closesocket(serverSocket);
        WSACleanup();

        return false;
    }

    cout << "[OK] Socket bound to Port 8080." << endl;

    return true;
}

// Listen
bool Server::startListening()
{
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Listen failed!" << endl;

        closesocket(serverSocket);
        WSACleanup();

        return false;
    }

    cout << "[OK] Listening..." << endl;

    return true;
}

// Accept Client
bool Server::acceptClient()
{
    cout << "Waiting for client..." << endl;

    clientSocket = accept(
        serverSocket,
        (sockaddr*)&clientAddress,
        &clientSize);

    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Accept failed!" << endl;

        closesocket(serverSocket);
        WSACleanup();

        return false;
    }

    cout << "[OK] Client Connected!" << endl;

    return true;
}

// Chat Function
void Server::chat()
{
    char buffer[1024];

    while (true)
    {
        ZeroMemory(buffer, sizeof(buffer));

        int bytesReceived =
            recv(clientSocket,
                 buffer,
                 sizeof(buffer),
                 0);

        if (bytesReceived <= 0)
        {
            cout << "Client disconnected." << endl;
            break;
        }

        cout << "\nClient : " << buffer << endl;

        if (strcmp(buffer, "/exit") == 0)
        {
            cout << "Client ended the chat." << endl;
            break;
        }

        string reply;

        cout << "Server : ";
        getline(cin, reply);

        send(clientSocket,
             reply.c_str(),
             reply.length(),
             0);

        if (reply == "/exit")
        {
            cout << "Closing chat..." << endl;
            break;
        }
    }
}

// Cleanup
void Server::cleanup()
{
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    WSACleanup();

    cout << "Server shutdown completed." << endl;
}



/*#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    cout << "==================================" << endl;
    cout << "   WhatsApp Server Starting..." << endl;
    cout << "==================================" << endl;

    // Structure to store Winsock information
    WSADATA wsa;

    // Initialize Winsock version 2.2
    int result = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (result != 0)
    {
        cout << "WSAStartup failed!" << endl;
        return 1;
    }

    cout << "Winsock initialized successfully." << endl;
    // Step 2: Create Socket 
     SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed!" << endl;
        WSACleanup();
        return 1;
    }

    cout << "Socket created successfully." << endl;
    
    // Step 3: Configure Server Address
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Step 4: Bind Socket
    if (bind(serverSocket,
             (sockaddr*)&serverAddress,
             sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cout << "Bind failed! Error Code: "
             << WSAGetLastError() << endl;

        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Socket bound successfully to Port 8080." << endl;

    // Step 5: Listen for incoming connections
if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
{
    cout << "Listen failed! Error Code: "
         << WSAGetLastError() << endl;

    closesocket(serverSocket);
    WSACleanup();
    return 1;
}

cout << "Server is listening on Port 8080..." << endl;
cout << "Waiting for a client to connect..." << endl;
     // Step 6: Accept Client
    sockaddr_in clientAddress;
    int clientSize = sizeof(clientAddress);

    SOCKET clientSocket = accept(
        serverSocket,
        (sockaddr*)&clientAddress,
        &clientSize);

    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Accept failed! Error Code: "
             << WSAGetLastError() << endl;

        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "[OK] Client connected successfully!" << endl;

    char buffer[1024] = {0};

//int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
while (true)
{
    ZeroMemory(buffer, sizeof(buffer));

    int bytesReceived = recv(clientSocket,
                             buffer,
                             sizeof(buffer),
                             0);

    if (bytesReceived <= 0)
    {
        cout << "Client Disconnected." << endl;
        break;
    }

    cout << "\nClient : " << buffer << endl;

    string reply;

    cout << "Server : ";
    getline(cin, reply);

    string message;
getline(cin, message);

if (message == "exit")
{
    send(clientSocket,
         message.c_str(),
         message.length(),
         0);

    break;
}

    send(clientSocket,
         reply.c_str(),
         reply.length(),
         0);
}

    // Close client socket
    closesocket(clientSocket);

    closesocket(serverSocket);
      // Cleanup Winsock
    WSACleanup();
    cout << "Server shutting down..." << endl;
    return 0;
}*/