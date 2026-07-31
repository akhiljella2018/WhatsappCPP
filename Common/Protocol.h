#pragma once
#include <string>

using namespace std;

enum class MessageType
{
    LOGIN,
    LOGIN_RESPONSE,      // NEW
    REGISTER,
    CHAT,
    PRIVATE_MESSAGE,
    USERS,
    SERVER_MESSAGE,      // NEW
    ERROR_MESSAGE,       // NEW
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