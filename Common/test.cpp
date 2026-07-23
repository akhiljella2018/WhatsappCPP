#include <iostream>
#include "Protocol.h"

using namespace std;

int main()
{
    Message msg;

    msg.type = MessageType::CHAT;
    msg.sender = "Akhil";
    msg.receiver = "";
    msg.text = "Hello Everyone";

    string data = serialize(msg);

    cout << "Serialized : " << data << endl;

    Message received = deserialize(data);

    cout << "\nAfter Deserialization\n";

    cout << "Sender   : " << received.sender << endl;
    cout << "Receiver : " << received.receiver << endl;
    cout << "Message  : " << received.text << endl;

    return 0;
}