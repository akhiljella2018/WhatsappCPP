#pragma once
#include <string>

using namespace std;

enum class MessageType
{
    LOGIN,
    CHAT,
    PRIVATE_MESSAGE,
    USERS,
    EXIT
};

struct Message
{
    MessageType type = MessageType::CHAT;
    string sender = "";
    string receiver = "";
    string text = "";
    string timestamp;
    string password = "";  
};

// Convert Message → String
string serialize(const Message& msg);

// Convert String → Message
Message deserialize(const string& data);

string getCurrentTimestamp();