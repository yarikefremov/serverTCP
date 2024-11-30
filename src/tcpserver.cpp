#include "../include/tcpserver.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>

// флаг для выхода из бесконечного цикла, используется при CTRL + C и других
// комбинациях, в идеале так и надо закрывать сервер
int volatile exitFlag = 1;

TCPServer::TCPServer() {
    SetConsoleCtrlHandler(TCPServer::ctrlHandler, TRUE) ? printf("\nThe Control Handler is installed.\n") : printf("\nERROR: Could not set control handler");
}

TCPServer::TCPServer (std::string ip_address, int port)
    : m_listener_ip_address (ip_address), m_listener_port (port)
{
  SetConsoleCtrlHandler (TCPServer::ctrlHandler, TRUE)
      ? printf ("\nThe Control Handler is installed.\n")
      : printf ("\nERROR: Could not set control handler");
}

TCPServer::~TCPServer () { WSACleanup (); }

bool TCPServer::initWinsock() {

    WSADATA data;
    WORD ver = MAKEWORD(2, 2);

    int wsInit = WSAStartup(ver, &data);

    if (wsInit != 0) {
        std::cout << "Error: can't initialize Winsock." << std::endl;
        return false;
    }

    return true;

}

bool TCPServer::createSocket() {

  // AF_INET = IPv4, SOCK_STREAM = TCP/IP
  m_listening_socket = socket (AF_INET, SOCK_STREAM, 0);

  // Если не получилось создать, то выходим из функции (скорее всего надо
  // выводить какую-то ошибку, но я не буду)
  if (m_listening_socket == INVALID_SOCKET)
    return 0;

  // Заполнение sockaddr_in структуры, которая хранит тип протокола (IPv4),
  // адрес и порт
  m_hint.sin_family = AF_INET;
  m_hint.sin_port = htons (m_listener_port);
  inet_pton (AF_INET, m_listener_ip_address.c_str (), &m_hint.sin_addr);

  // Попытка занять сокет, bind используется только для сервера, клиент
  // привязывать так нежелательно
  int bindCheck
      = bind (m_listening_socket, (sockaddr *)&m_hint, sizeof (m_hint));

  // Если не получилось привязать, значит там уже кто-то есть
  if (bindCheck == SOCKET_ERROR)
    return 0;

  // Говорим сокету, что он прослушивающий, не ограничиваем количество
  // подключений
  int listenCheck = listen (m_listening_socket, SOMAXCONN);
  if (listenCheck == SOCKET_ERROR)
    return 0;

  return m_listening_socket;
}

void TCPServer::run() {

  // Создаем сокет
  createSocket ();

  // Если не удалось создать сокет, то программа закрывается
  if (m_listening_socket == INVALID_SOCKET)
    return;

  // Опустошаем наше хранилище сокетов
  FD_ZERO (&m_master);

  // Добавляем туда сокет, созданный для прослушки, в дальнейшем, когда
  // будет обработка сокетов, он нам понадобится
  FD_SET (m_listening_socket, &m_master);

  // Пока ctrlHandler не изменил флаг цикл работает. Вероятно, можно было
  // сделать отдельный поток, чтобы ловить выключение терминала или CTRL+C, но
  // кажется, это излишнее
  while (exitFlag)
    {

      // m_master - набор сокетов, редактируемый программой сервера,
      // m_copy - набор сокетов, редактируемый командой select
      // В рамках экономии была попытка использовать только m_master,
      // но select записывает в m_copy только часть сокетов (готовые для
      // прослушивания), т.е. часть теряется
      m_copy = m_master;
      int socketCount = select (0, &m_copy, nullptr, nullptr, &m_timeout);

      // Обработка сокетов может быть довольно долгой, поэтому ради экономии
      // времени проводим дополнительную проверку, чтобы сервер закрылся как
      // можно быстрее после CTRL+C
      if (!exitFlag)
        break;

      // Обработка набора сокетов m_copy
      socketHandle (socketCount);
    }

  // Выключаем сокет, он нам больше не нужен
  shutdown (m_listening_socket, SD_BOTH);
}

void TCPServer::socketHandle(int socketCount){

  // Пробегаемся по набору сокетов m_copy
  for (int i = 0; i < socketCount; ++i)
    {

      // берем сокет
      m_copy_sock = m_copy.fd_array[i];

      // Если серверный сокет - значит новое подключение, там создается еще
      // один сокет, который мы добавляем в мастер, таким образом можно
      // подключить много клиентов, по умолчанию в Winsock2 FD_SETSIZE = 64,
      // так что 64 клиента
      if (m_copy_sock == m_listening_socket)
        {

          // Интересный факт, если сразу вставить accept в FD_SET, то серверная
          // программа не будет работать
          m_client_sock = accept (m_listening_socket, nullptr, nullptr);
          FD_SET (m_client_sock, &m_master);
          continue;
        }

      // В случае если мы получаем пакет на какой-то сокет

      // Очищаем буфер, прежде чем в него что-то писать, кстати эта функция
      // используется в tcpclient только один раз, а не в цикле
      ZeroMemory (m_buf, MAX_BUFFER_SIZE);

      // На сервере не обязательно ставить таймаут на recv, так как нам уже
      // известно, что можно считать какие-то данные с этого сокета
      int bytesReceived = recv (m_copy_sock, m_buf, MAX_BUFFER_SIZE, 0);

      // Если мы получили -1 или 0 в результате recv, то, наверное, клиент
      // хочет завершить передачу данных
      if (bytesReceived <= 0)
        {

          // Закрываем сокет, удаляем его из нашего хранилища
          closesocket (m_copy_sock);
          FD_CLR (m_copy_sock, &m_master);

          // Если мы нашли наш сокет в таблице, значит он был уже авторизирован
          // в системе и нам его надо удалить оттуда, это не удалит его из db
          // так как там своя таблица, редактируемая в самом DBTxt
          if (m_socket_map.find (m_copy_sock) != 0)
            {
              m_name_map.erase (m_socket_map[m_copy_sock]);
              m_socket_map.erase (m_copy_sock);
            }
          continue;
        }

      // Нам может прийти два пакета - auth и msg
      // Auth
      if (m_buf[0] == FLAG_AUTH_PACK)
        {

          // Проверяем регистрируется или логинится пользователь, попутно
          // записывая этот флаг в acceptpckg, кстати, acceptpckg.loginflag
          // задумывался так, чтобы проверять не перепутано что-то где-то, но
          // по итогу нигде не использовался
          if ((m_answer_packet.loginflag = m_buf[1]) == 0u)
            {

              // Пробуем зарегистрировать пользователя в базе
              // m_buf + 2 - указатель на начало имени
              // m_buf + 2 + 33 - указатель на начало пароля
              // Неплохо было бы запихнуть это в define, но пока не буду
              // db возвращает успех или неудачу, что записывается сразу в
              // пакет
              m_answer_packet.ans = m_db.registerUser (
                  std::string (m_buf + 2), std::string (m_buf + 2 + 33));

              // Если получилось зарегистрироваться, то добавляем пользователя
              // в обе таблицы. Таблицу можно было использовать одну, но поиск
              // ключа по значению довольно трудоемкое занятие, с учетом того,
              // что пользователя надо искать КАЖДЫЙ РАЗ, как приходит
              // сообщение, так как клиент не знает сокет другого пользователя,
              // а только имя
              if (m_answer_packet.ans)
                {
                  m_socket_map[m_copy_sock] = std::string (m_buf + 2);
                  m_name_map[std::string (m_buf + 2)] = m_copy_sock;
                }

              // Отладочная информация, потом можно удалить
              std::cout << "Socket " << m_copy_sock << ": "
                        << m_answer_packet.loginflag << " "
                        << m_answer_packet.ans;

              // Отправляет результат клиенту
              send (m_copy_sock, (const char *)(&m_answer_packet),
                    sizeof (m_answer_packet), 0);
            }

          // Если пользователь логинится
          else if (m_buf[1] == 1u)
            {

              // Также пытаемся его залогинить в db, если в db нет такого имени
              // или пароль не подходит, то будет возвращен 0
              m_answer_packet.ans = m_db.loginUser (
                  std::string (m_buf + 2), std::string (m_buf + 2 + 33));

              // Если пользователь уже есть в сети, то мы не позволяем ему
              // заходить второй раз, так как тогда, поведение сервера по
              // отправке сообщений не определено, скорее всего он будет
              // отсылать сообщения только на один сокет или же часть на один,
              // часть на другой + m_name_map сокет пользователя будет
              // перезаписан, т.е. сервер будет знать только об одном сокете
              if (m_name_map.find (std::string (m_buf + 2)) != 0)
                {
                  m_answer_packet.ans = 0;
                }

              // Если все прошло хорошо, то записываем пользователя в таблички
              if (m_answer_packet.ans)
                {
                  m_socket_map[m_copy_sock] = std::string (m_buf + 2);
                  m_name_map[std::string (m_buf + 2)] = m_copy_sock;
                }

              // Отладочная информация, в дальнейшем можно будет убрать
              std::cout << "Socket " << m_copy_sock << ": "
                        << m_answer_packet.loginflag << " "
                        << m_answer_packet.ans;

              // Отправляем результат клиенту
              send (m_copy_sock, (const char *)(&m_answer_packet),
                    sizeof (m_answer_packet), 0);
            }
        }

      // Если пришел пакет сообщения, то просто отправляем его нужному клиенту
      else if (m_buf[0] == FLAG_MSG_PACK)
        {

          // Отладочная информация, потом можно убрать
          // m_buf + 68 указатель на msgpckg.msg
          std::cout << std::string (m_buf + 68, sizeof (m_buf) - 68);

          // Отправляем нужному клиенту
          // m_buf+34 - указатель на msgpckg.dstname
          // предполагается, что имя ВСЕГДА заканчивается \0
          send (m_name_map[std::string (m_buf + 34)], m_buf, sizeof (m_buf),
                0);
        }
    }
}

// Готовая функция из microsoft docs, чтобы обрабатывать нажатия CTRL в
// терминале Сервер будет корректно выключаться ТОЛЬКО в случае использования
// этой функции
int __stdcall TCPServer::ctrlHandler (DWORD fdwCtrlType)
{
  switch (fdwCtrlType)
    {
      // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
      exitFlag = 0;
      return 1;

      // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
      exitFlag = 0;
      return 1;

      // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
      exitFlag = 0;
      return 0;

    case CTRL_LOGOFF_EVENT:
      exitFlag = 0;
      return 0;

    case CTRL_SHUTDOWN_EVENT:
      exitFlag = 0;
      return 0;

    default:
      return 0;
    }
}
