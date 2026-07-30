#include "Authentication.h"

#include <fstream>
#include <sstream>

bool Authentication::login(
    const string& username,
    const string& password)
{
    ifstream file("Data/users.txt");

    if (!file.is_open())
    {
        return false;
    }

    string line;

    while (getline(file, line))
    {
        stringstream ss(line);

        string user;
        string pass;

        getline(ss, user, ',');
        getline(ss, pass);

        if (user == username &&
            pass == password)
        {
            return true;
        }
    }

    return false;
}