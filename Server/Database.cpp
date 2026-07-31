#include "Database.h"

#include <iostream>

using namespace std;

Database::Database()
{
    db = nullptr;
}

Database::~Database()
{
    disconnect();
}

bool Database::connect(const string& dbName)
{
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK)
    {
        cout << "Cannot open database!" << endl;
        return false;
    }

    cout << "[OK] Database Connected." << endl;
    return true;
}

void Database::disconnect()
{
    if (db != nullptr)
    {
        sqlite3_close(db);
        db = nullptr;

        cout << "[OK] Database Closed." << endl;
    }
}

bool Database::createTables()
{
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE,"
        "password TEXT);";

    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db,
        sql,
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK)
    {
        cout << "SQL Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }

    cout << "[OK] Users table ready." << endl;
    return true;
}

bool Database::registerUser(const string& username, const string& password)
{
    return false;
}

bool Database::login(const string& username, const string& password)
{
    return false;
}