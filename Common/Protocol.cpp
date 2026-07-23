#include "Protocol.h"

#include <sstream>
#include <vector>

using namespace std;

string serialize(const Message& msg)
{
    string type;

    switch(msg.type)
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

        case MessageType::EXIT:
            type = "EXIT";
            break;
    }

    return type + "|" +
           msg.sender + "|" +
           msg.receiver + "|" +
           msg.text;
}

Message deserialize(const string& data)
{
    Message msg;

    stringstream ss(data);

    string token;

    vector<string> parts;

    while (getline(ss, token, '|'))
    {
        parts.push_back(token);
    }

    if (parts.size() < 4)
    {
        return msg;
    }

    if (parts[0] == "LOGIN")
        msg.type = MessageType::LOGIN;

    else if (parts[0] == "CHAT")
        msg.type = MessageType::CHAT;

    else if (parts[0] == "PRIVATE")
        msg.type = MessageType::PRIVATE_MESSAGE;

    else if (parts[0] == "EXIT")
        msg.type = MessageType::EXIT;

    msg.sender = parts[1];
    msg.receiver = parts[2];
    msg.text = parts[3];

    return msg;
}