#ifndef PASSCHECKER_H
#define PASSCHECKER_H
#include "sqlite3c.h"
#include <string>
#include <iostream>

class Passchecker
{
private:
    sqlite3* db;
    char* dberrmsg;
    std::string sql;
    int result;
    static int callback(void *notUsed, int colCount, char **columns, char **colNames);
public:
    Passchecker();
    ~Passchecker();
    bool isNewUser(const char* name);
    bool userRegister(const char* name, const char* password); // 0 - логин уже существует
    bool userLogin(const char* name, const char* password); // 0 - имя или пароль не существует
    //что должны передавать? Вот на сервер приходит пакет с логином и паролем
    //мы можем принимать пароль и внутри его обрабатывать
    // НЕЕЕТ
    // СЕРВЕР ОБРАБАТЫВАЕТ НОВЫЙ ЛИ ПОЛЬЗОВАТЕЛЬ ИЛИ НЕТ
    // ONLY ENGLISH WORDS
};

#endif // PASSCHECKER_H
