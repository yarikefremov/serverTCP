#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <WS2tcpip.h>
#include "../include/dbtxt.h"
#include "../include/pckg.h"

#define MAX_BUFFER_SIZE (sizeof(msgpckg))

class TCPServer {
public:
    TCPServer();
    TCPServer (std::string ip_address, int port);
    ~TCPServer();
    bool initWinsock ();
    void run();


private:
  // Функция обработки нажатий CTRL в терминале, сервер корректно закрывается
  // только в случае использования данной функции
  static int ctrlHandler (DWORD fdwCtrlType);

  // Создание сокетов + обработка пакетов, приходящих на сокеты
  bool createSocket ();
  void socketHandle (int socket_count);

  // Класс базы данных в виде txt файла
  DBTxt m_db;

  // Таблицы для поиска сокета по имени и наоборот
  // Операция поиска возникает при получении КАЖДОГО сообщения, поэтому таблицы
  // 2, а не одна
  std::unordered_map<SOCKET, std::string> m_socket_map;
  std::unordered_map<std::string, SOCKET> m_name_map;

  // Массивы сокетов, master - для редактирования программой, copy для
  // редактирования функцией select из Winsok2
  fd_set m_master;
  fd_set m_copy;

  // IP и порт сервера
  std::string m_listener_ip_address;
  int m_listener_port;

  // Структура хранящая тип протокола, адрес и порт
  sockaddr_in m_hint;

  // Буфер куда записывается содержимое любого пакета при обработке
  char m_buf[MAX_BUFFER_SIZE];

  // Сокеты для работы 1 - слушающий (туда приходят все новые соединения), 2 -
  // нужен для итерации по copy, 3 - в него записывается результат выполнения
  // функции accept
  SOCKET m_listening_socket;
  SOCKET m_copy_sock;
  SOCKET m_client_sock;

  // таймаут для select, так как select может очень долго крутиться, если
  // вообще нет соединений
  TIMEVAL m_timeout{ 1, 0 };

  // Пакет ответа клиенту в случае аутентификации
  acceptpckg m_answer_packet;
};

#endif // TCPSERVER_H
