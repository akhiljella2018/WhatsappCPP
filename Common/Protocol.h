#pragma once

#include <string>

using namespace std;

enum class MessageType
{
    LOGIN,
    CHAT,
    PRIVATE_MESSAGE,
    EXIT
};

struct Message
{
    MessageType type;

    string sender;

    string receiver;

    string text;
};

// Convert Message → String
string serialize(const Message& msg);

// Convert String → Message
Message deserialize(const string& data);