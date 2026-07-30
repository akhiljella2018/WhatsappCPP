#include "ChatHistory.h"

void ChatHistory::addMessage(const Message& msg)
{
    history.push_back(msg);

    if (history.size() > 50)
    {
        history.erase(history.begin());
    }
}

vector<Message> ChatHistory::getMessages() const
{
    return history;
}

void ChatHistory::clear()
{
    history.clear();
}