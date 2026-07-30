#pragma once

#include <vector>
#include "../Common/Protocol.h"

using namespace std;

class ChatHistory
{
private:
    vector<Message> history;

public:
    void addMessage(const Message& msg);

    vector<Message> getMessages() const;

    void clear();
};