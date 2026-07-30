#include "Client.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <atomic>
#include "../Common/Protocol.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>


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

    // Ask username
    cout << "Enter Username : ";
    getline(cin, username); 

    string password;

    cout << "Enter Password : ";
    getline(cin, password);
        // Create LOGIN packet
        Message login;

    login.type = MessageType::LOGIN;
    login.sender = username;
    login.receiver = "";
    login.text = "";
    login.timestamp = getCurrentTimestamp();
    login.password = password;  
    string packet = serialize(login);

    cout << "\nSending LOGIN Packet" << endl;
    cout << "Username : " << login.sender << endl;
    cout << "Password : " << login.password << endl;

// Send LOGIN packet
send(clientSocket,
     packet.c_str(),
     static_cast<int>(packet.length()),
     0);

cout << "[OK] Login packet sent." << endl;

    return true;
}
// Create Socket
bool Client::createSocket()
{
    clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

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

string getCurrentTime()
{
    auto now = chrono::system_clock::now();

    time_t currentTime = chrono::system_clock::to_time_t(now);

    tm localTime;

    localtime_s(&localTime, &currentTime);

    stringstream ss;

    ss << put_time(&localTime, "%H:%M:%S");

    return ss.str();
}

void receiveMessages(SOCKET clientSocket)
{
    char buffer[1024];
    string receiveBuffer;

    while (running)
    {
        ZeroMemory(buffer, sizeof(buffer));

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytesReceived <= 0)
        {
            lock_guard<mutex> lock(consoleMutex);

            cout << "\nServer disconnected." << endl;

            running = false;
            break;
        }

        buffer[bytesReceived] = '\0';

        receiveBuffer += buffer;

        while (true)
        {
            size_t pos = receiveBuffer.find('\n');

            if (pos == string::npos)
                break;

            string packet = receiveBuffer.substr(0, pos);

            receiveBuffer.erase(0, pos + 1);

            if (packet.empty())
                continue;

            Message msg = deserialize(packet);

            lock_guard<mutex> lock(consoleMutex);

            switch (msg.type)
            {
                case MessageType::CHAT:
                    cout << "\n["
                         << msg.timestamp
                         << "] "
                         << msg.sender
                         << " : "
                         << msg.text
                         << endl;
                    break;

                case MessageType::PRIVATE_MESSAGE:
                    cout << "\n[PRIVATE] "
                         << msg.timestamp
                         << " "
                         << msg.sender
                         << " : "
                         << msg.text
                         << endl;
                    break;

                case MessageType::USERS:
                    cout << "\n"
                         << msg.text
                         << endl;
                    break;

                default:
                    break;
            }
        }
    }
}
void Client::chat()
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

        Message chat;
        chat.sender = username;
        chat.timestamp = getCurrentTime();

        if (message.rfind("/msg ", 0) == 0)
        {
            chat.type = MessageType::PRIVATE_MESSAGE;

            size_t firstSpace = message.find(' ', 5);

            if (firstSpace == string::npos)
            {
                cout << "Usage: /msg <username> <message>" << endl;
                continue;
            }

            chat.receiver = message.substr(5, firstSpace - 5);
            chat.text = message.substr(firstSpace + 1);
        }
        else if (message == "/users")
        {
            chat.type = MessageType::USERS;
            chat.receiver = "";
            chat.text = "";
        }
        else if (message == "/exit")
        {
            chat.type = MessageType::EXIT;
            chat.receiver = "";
            chat.text = "";
        }
        else
        {
            chat.type = MessageType::CHAT;
            chat.receiver = "";
            chat.text = message;
        }

        string packet = serialize(chat);

        send(
            clientSocket,
            packet.c_str(),
            static_cast<int>(packet.length()),
            0
        );

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