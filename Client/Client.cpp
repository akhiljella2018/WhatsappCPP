#include "Client.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <atomic>

using namespace std;

mutex consoleMutex;
atomic<bool> running(true);

// Constructor
Client::Client()
{
    clientSocket = INVALID_SOCKET;
}

// Destructor
Client::~Client()
{
    cleanup();
}

// Initialize
bool Client::initialize()
{
    cout << "==================================" << endl;
    cout << "     WhatsApp Client Starting     " << endl;
    cout << "==================================" << endl;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "WSAStartup Failed!" << endl;
        return false;
    }

    cout << "[OK] Winsock Initialized." << endl;

    if (!createSocket())
        return false;

    if (!connectToServer())
        return false;

    return true;
}

// Create Socket
bool Client::createSocket()
{
    clientSocket = socket(AF_INET,
                          SOCK_STREAM,
                          IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Socket Creation Failed!" << endl;
        WSACleanup();
        return false;
    }

    cout << "[OK] Socket Created." << endl;

    return true;
}

// Connect to Server
bool Client::connectToServer()
{
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket,
                (sockaddr*)&serverAddress,
                sizeof(serverAddress))
        == SOCKET_ERROR)
    {
        cout << "Connection Failed! Error Code : "
             << WSAGetLastError()
             << endl;

        closesocket(clientSocket);
        WSACleanup();

        return false;
    }

    cout << "[OK] Connected to Server!" << endl;

    return true;
}

void receiveMessages(SOCKET clientSocket)
{
    char buffer[1024];

    while (running)
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
            lock_guard<mutex> lock(consoleMutex);

            cout << "\nServer disconnected." << endl;

            running = false;
            break;
        }


        {
            lock_guard<mutex> lock(consoleMutex);

            cout << "\nServer : "
                 << buffer
                 << endl;
        }


        if (strcmp(buffer, "/exit") == 0)
        {
            running = false;
            break;
        }
    }
}void Client::chat()
{
    thread receiver(receiveMessages, clientSocket);


    while (running)
    {
        string message;


        {
            lock_guard<mutex> lock(consoleMutex);

            cout << "\nYou : ";
        }


        getline(cin, message);


        send(clientSocket,
             message.c_str(),
             message.length(),
             0);


        if (message == "/exit")
        {
            running = false;
            break;
        }
    }


    receiver.join();
}
// Cleanup
void Client::cleanup()
{
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    WSACleanup();

    cout << "Client shutdown completed." << endl;
}