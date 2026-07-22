#include "Server.h"

int main()
{
    Server server;

    if (!server.initialize())
        return 1;

    server.chat();

    //server.cleanup();

    return 0;
}