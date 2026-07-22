#include "Client.h"

int main()
{
    Client client;

    if (!client.initialize())
        return 1;

    client.chat();

    return 0;
}










/*#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    cout << "==================================" << endl;
    cout << "      WhatsApp Client" << endl;
    cout << "==================================" << endl;

    // Step 1: Initialize Winsock
    WSADATA wsa;

    int result = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (result != 0)
    {
        cout << "WSAStartup failed!" << endl;
        return 1;
    }

    cout << "[OK] Winsock initialized." << endl;

    // Step 2: Create Client Socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed!" << endl;
        WSACleanup();
        return 1;
    }

    cout << "[OK] Client socket created." << endl;

    // Step 3: Server Address
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Step 4: Connect to Server
    if (connect(clientSocket,
                (sockaddr*)&serverAddress,
                sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cout << "Connection failed! Error: "
             << WSAGetLastError() << endl;

        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "[OK] Connected to Server Successfully!" << endl;
    string message;

    cout << "\nEnter Message : ";
    getline(cin, message);


     char buffer[1024];

while (true)
{
    string message;

    cout << "\nYou : ";
    getline(cin, message);

    send(clientSocket,
         message.c_str(),
         message.length(),
         0);

    ZeroMemory(buffer, sizeof(buffer));

    int bytesReceived =
        recv(clientSocket,
             buffer,
             sizeof(buffer),
             0);

    if (bytesReceived <= 0)
    {
        cout << "Server disconnected.\n";
        break;
    }
    if (strcmp(buffer, "exit") == 0)
{
    cout << "Client ended the chat." << endl;
    break;
}
    cout << "Server : " << buffer << endl;
}

cout << "Message Sent Successfully!" << endl;

    closesocket(clientSocket);

    WSACleanup();

    return 0;
}*/