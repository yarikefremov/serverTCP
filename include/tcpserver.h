#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <WS2tcpip.h>

class TCPServer;

//Callback fct = fct with fct as parameter.
typedef void(*MessageReceivedHandler)(TCPServer* listener, int socketID, std::string msg);

class TCPServer {
public:
    TCPServer();
    TCPServer(std::string ipAddress, int port);
    ~TCPServer();

    void sendMsg(int clientSocket, std::string msg);
    bool initWinsock();
    void run();
    void cleanupWinsock();


private:
    SOCKET createSocket();
    std::string listenerIPAddress;
    int listenerPort;
    //MessageReceivedHandler messageReceived;
};

#endif // TCPSERVER_H
