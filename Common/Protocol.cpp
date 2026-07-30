#include "Protocol.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

string serialize(const Message& msg)
{
    string type;

    switch (msg.type)
    {
        case MessageType::LOGIN:
            type = "LOGIN";
            break;

        case MessageType::CHAT:
            type = "CHAT";
            break;

        case MessageType::PRIVATE_MESSAGE:
            type = "PRIVATE";
            break;

        case MessageType::USERS:
            type = "USERS";
            break;

        case MessageType::EXIT:
            type = "EXIT";
            break;
    }

    return type + "|" +
           msg.sender + "|" +
           msg.receiver + "|" +
           msg.timestamp + "|" +
           msg.text + "\n";
}

Message deserialize(const string& data)
{
    cout << "\n========== DESERIALIZE ==========" << endl;
    cout << "RAW DATA : [" << data << "]" << endl;

    Message msg;

    stringstream ss(data);
    string token;
    vector<string> parts;

    while (getline(ss, token, '|'))
    {
        parts.push_back(token);
    }

    cout << "Parts Count = " << parts.size() << endl;

    for (size_t i = 0; i < parts.size(); i++)
    {
        cout << "Part[" << i << "] = [" << parts[i] << "]" << endl;
    }

    /*if (parts.size() < 4)
    {
        cout << "ERROR : Invalid Packet!" << endl;
        return msg;
    }*/
    while (parts.size() < 5)
{
    parts.push_back("");
}

    cout << "\nComparisons" << endl;
    cout << "LOGIN   : " << (parts[0] == "LOGIN") << endl;
    cout << "CHAT    : " << (parts[0] == "CHAT") << endl;
    cout << "PRIVATE : " << (parts[0] == "PRIVATE") << endl;
    cout << "USERS   : " << (parts[0] == "USERS") << endl;
    cout << "EXIT    : " << (parts[0] == "EXIT") << endl;

    if (parts[0] == "LOGIN")
    {
        msg.type = MessageType::LOGIN;
    }
    else if (parts[0] == "CHAT")
    {
        msg.type = MessageType::CHAT;
    }
    else if (parts[0] == "PRIVATE")
    {
        msg.type = MessageType::PRIVATE_MESSAGE;
    }
    else if (parts[0] == "USERS")
    {
        msg.type = MessageType::USERS;
    }
    else if (parts[0] == "EXIT")
    {
        msg.type = MessageType::EXIT;
    }
    else
    {
        cout << "UNKNOWN MESSAGE TYPE!" << endl;
    }

    msg.sender   = parts[1];
    msg.receiver = parts[2];
    msg.timestamp = parts[3];
    msg.text     = parts[4];

    cout << "\nDecoded Message" << endl;
    cout << "Type     : " << static_cast<int>(msg.type) << endl;
    cout << "Sender   : " << msg.sender << endl;
    cout << "Receiver : " << msg.receiver << endl;
    cout << "Text     : " << msg.text << endl;
    cout << "===============================\n" << endl;

    return msg;
}
#include <chrono>
#include <ctime>
#include <iomanip>

string getCurrentTimestamp()
{
    auto now = chrono::system_clock::now();

    time_t current = chrono::system_clock::to_time_t(now);

    tm localTime{};
    localtime_s(&localTime, &current);

    stringstream ss;
    ss << put_time(&localTime, "%H:%M:%S");

    return ss.str();
}