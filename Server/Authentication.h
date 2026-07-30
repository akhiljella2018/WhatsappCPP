#pragma once

#include <string>

using namespace std;

class Authentication
{
public:
    bool login(
        const string& username,
        const string& password
    );
};