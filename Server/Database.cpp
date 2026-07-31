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
    registerUser("Akhil", "1234");
    registerUser("Priya", "abcd");
    registerUser("Rahul", "pass123");
    return true;
}

bool Database::registerUser(const string& username,
                            const string& password)
{
    const char* sql =
        "INSERT INTO users(username,password) VALUES(?,?);";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cout << "Prepare failed!" << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);

    return success;
}

bool Database::login(const string& username,
                     const string& password)
{
    const char* sql =
        "SELECT password FROM users WHERE username=?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool success = false;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        string dbPassword =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt, 0));

        if (dbPassword == password)
        {
            success = true;
        }
    }

    sqlite3_finalize(stmt);

    return success;
}