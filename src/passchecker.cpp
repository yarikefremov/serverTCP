#include "../include/passchecker.h"
#include <cstring>
#include <stdio.h>

Passchecker::Passchecker() {
    result  = sqlite3_open("test.db", &db);
    // если подключение успешно установлено
    if(result == SQLITE_OK)
    {
        printf("Connection established\n");
    }
    else
    {
        // выводим сообщение об ошибке
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sql = "CREATE TABLE IF NOT EXISTS people(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, pass TEXT);";
    result = sqlite3_exec(db, sql.c_str(), 0, 0, &dberrmsg);
    if (result != SQLITE_OK )
    {
        printf("SQL error 1: %s\n", dberrmsg);
        sqlite3_free(dberrmsg); // очищаем ресурсы
    }
    else printf("Table created\n");
    sql = new char[256];
}

Passchecker::~Passchecker(){
    sqlite3_close(db);
}

bool Passchecker::isNewUser(const char* name){
    // получаем все данные из таблицы people
    sql = "SELECT * FROM people";
    result = sqlite3_exec(db, sql.c_str(), callback, 0, &dberrmsg);
    if (result != SQLITE_OK )
    {
        printf("SQL error: %s\n", dberrmsg);
        sqlite3_free(dberrmsg);
    }
    return 0;
}

bool Passchecker::userRegister(const char* name, const char* password){
    //if(!isNewUser(name)) return 0;

    sql = "INSERT INTO people (name, pass) VALUES (\"" + std::string(name) + "\",\"" + std::string(password) + "\");";
    std::cout<<sql;
    result = sqlite3_exec(db, sql.c_str(), 0, 0, &dberrmsg);
    if (result != SQLITE_OK )
    {
        printf("SQL error 2: %s\n", dberrmsg);
        sqlite3_free(dberrmsg); // очищаем ресурсы
    }
    return 1;
}

int Passchecker::callback(void *notUsed, int colCount, char **columns, char **colNames)
{
    for (int i = 0; i < colCount; i++)
    {
        printf("%s = %s\n", colNames[i], columns[i] ? columns[i] : "NULL");
    }
    printf("\n");
    return 0;
}
