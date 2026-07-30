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
        if (!pass.empty() && pass.back() == '\r')
{
    pass.pop_back();
}

if (!password.empty() && password.back() == '\n')
{
    // Only if you receive '\n' in the password
}

        if (user == username &&
            pass == password)
        {
            return true;
        }
    }

    return false;
}