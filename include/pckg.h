#ifndef PCKG_H
#define PCKG_H

// Размер полей пакетов
#define NAME_LENGHT 33
#define PASSWORD_LENGHT 33
#define MSG_LENGHT 100

// Значения флагов идентификации пакетов
#define FLAG_AUTH_PACK 0u
#define FLAG_MSG_PACK 1u
#define FLAG_ACCEPT_FLAG 2u

// Пакет для регистрации
struct authpckg
{
  unsigned char flag = FLAG_AUTH_PACK;
  bool loginflag; // 0 - register, 1 - login
  char username[NAME_LENGHT];
  char password[PASSWORD_LENGHT];
};

// Пакет для входящих и исходящих сообщений
struct msgpckg
{
  unsigned char flag = FLAG_MSG_PACK;
  char srcname[NAME_LENGHT];
  char dstname[NAME_LENGHT];
  bool rsrv;
  char msg[MSG_LENGHT];
};

// Пакет для ответа сервера при регистрации
struct acceptpckg
{
  unsigned char flag = FLAG_ACCEPT_FLAG;
  bool loginflag; // 0 - register, 1 - login
  bool ans;       // 0 - no, 1 - yes
};

#endif // PCKG_H
