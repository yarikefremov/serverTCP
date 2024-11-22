#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <WS2tcpip.h>
#include "../include/dbtxt.h"
#include "../include/pckg.h"

#define MAX_BUFFER_SIZE (sizeof(msgpckg))

class TCPServer;

//Callback fct = fct with fct as parameter.
typedef void(*MessageReceivedHandler)(TCPServer* listener, int socketID, std::string msg);

class TCPServer {
public:
    TCPServer();
    TCPServer(std::string ipAddress, int port);
    ~TCPServer();

    void sendMsg(int clientSocket, msgpckg msg);
    bool initWinsock();
    void run();
    void cleanupWinsock();


private:
    bool createSocket();
    std::string listenerIPAddress;
    int listenerPort;
    SOCKET listeningSocket;
    DBTxt db;
    std::unordered_map<SOCKET, std::string> socketmap;
    std::unordered_map<std::string, SOCKET> namemap;
    static int ctrlHandler(DWORD fdwCtrlType);
    char buf[MAX_BUFFER_SIZE];
    TIMEVAL timeout{1, 0};
    fd_set master;
    fd_set copy;
    void socketHandle(int socketCount);

    acceptpckg answerpacket;
    //MessageReceivedHandler messageReceived;
};

#endif // TCPSERVER_H
