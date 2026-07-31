#pragma once

#include <string>
#include "../ThirdParty/sqlite/sqlite3.h"

using namespace std;

class Database
{
private:
    sqlite3* db;

public:
    Database();
    ~Database();

    bool connect(const string& dbName);
    void disconnect();

    bool createTables();

    bool registerUser(
        const string& username,
        const string& password
    );

    bool login(
        const string& username,
        const string& password
    );
};