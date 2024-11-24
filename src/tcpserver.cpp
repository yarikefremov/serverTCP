#include "../include/tcpserver.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>

int volatile exitFlag = 1;

int __stdcall TCPServer::ctrlHandler(DWORD fdwCtrlType)
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


TCPServer::TCPServer() {
    SetConsoleCtrlHandler(TCPServer::ctrlHandler, TRUE) ? printf("\nThe Control Handler is installed.\n") : printf("\nERROR: Could not set control handler");
}


TCPServer::TCPServer(std::string ipAddress, int port) : listenerIPAddress(ipAddress), listenerPort(port){
    SetConsoleCtrlHandler(TCPServer::ctrlHandler, TRUE) ? printf("\nThe Control Handler is installed.\n") : printf("\nERROR: Could not set control handler");
}

TCPServer::~TCPServer() {
    cleanupWinsock();			//Cleanup Winsock when the server shuts down.
}


//Function to check whether we were able to initialize Winsock & start the server.
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


//Function that creates a listening socket of the server.
bool TCPServer::createSocket() {

    listeningSocket = socket(AF_INET, SOCK_STREAM, 0);	//AF_INET = IPv4.

    if (listeningSocket != INVALID_SOCKET) {

        sockaddr_in hint;		//Structure used to bind IP address & port to specific socket.
        hint.sin_family = AF_INET;		//Tell hint that we are IPv4 addresses.
        hint.sin_port = htons(listenerPort);	//Tell hint what port we are using.
        inet_pton(AF_INET, listenerIPAddress.c_str(), &hint.sin_addr); 	//Converts IP string to bytes & pass it to our hint. hint.sin_addr is the buffer.

        int bindCheck = bind(listeningSocket, (sockaddr*)&hint, sizeof(hint));	//Bind listeningSocket to the hint structure. We're telling it what IP address family & port to use.

        if (bindCheck != SOCKET_ERROR) {			//If bind OK:

            int listenCheck = listen(listeningSocket, SOMAXCONN);	//Tell the socket is for listening.
            if (listenCheck == SOCKET_ERROR) {
                return -1;
            }
        }

        else {
            return -1;
        }

        return listeningSocket;

    }
    return -1;
}


//Function doing the main work of the server -> evaluates sockets & either accepts connections or receives data.
void TCPServer::run() {

    createSocket();		//Create the listening socket for the server.

    while (exitFlag) {

        if (listeningSocket == INVALID_SOCKET) break;

        FD_ZERO(&master);			//Empty file file descriptor.
        FD_SET(listeningSocket, &master);		//Add listening socket to file descriptor.

        while (exitFlag) {

            copy = master;	//Create new file descriptor bc the file descriptor gets destroyed every time.
            int socketCount = select(0, &copy, nullptr, nullptr, &timeout);				//Select() determines status of sockets & returns the sockets doing "work".
            if(!exitFlag) break;

            socketHandle(socketCount);
        }
    }
    shutdown(listeningSocket, SD_BOTH);
}


void TCPServer::cleanupWinsock() {

    WSACleanup();

}

void TCPServer::socketHandle(int socketCount){
    for (int i = 0; i < socketCount; ++i) {				//Server can only accept connection & receive msg from client.

        SOCKET sock = copy.fd_array[i];

        if (sock == listeningSocket) {				//Case 1: accept new connection.

            SOCKET client = accept(listeningSocket, nullptr, nullptr);
            FD_SET(client, &master);
        }
        else {										//Case 2: receive a msg.

            ZeroMemory(buf, MAX_BUFFER_SIZE);		//Clear the buffer before receiving data.
            int bytesReceived = recv(sock, buf, MAX_BUFFER_SIZE, 0);	//Receive data into buf & put it into bytesReceived.

            if (bytesReceived <= 0) {	//No msg = drop client.
                closesocket(sock);
                FD_CLR(sock, &master);	//Remove connection from file director.
                if(socketmap.find(sock) != 0){
                    namemap.erase(socketmap[sock]);
                    socketmap.erase(sock);
                }
            }

            else {						//Send msg to other clients & not listening socket.
                if(buf[0] == 0u){ //register or login
                    if((answerpacket.loginflag = buf[1]) == 0u){ //register
                        answerpacket.ans = db.registerUser(std::string(buf+2), std::string(buf+2+33));
                        //send(outSock);
                        if(answerpacket.ans){
                            socketmap[sock] = std::string(buf+2);
                            namemap[std::string(buf+2)] = sock;
                        }
                        std::cout<<"Socket "<<sock<<": "<<answerpacket.loginflag<<" "<<answerpacket.ans;
                        send(sock, (const char*)(&answerpacket), sizeof(answerpacket), 0);
                    }
                    else if(buf[1] == 1u){ //login
                        answerpacket.ans = db.loginUser(std::string(buf+2), std::string(buf+2+33));
                        if(namemap.find(std::string(buf+2)) != 0){ //User is already connected
                            answerpacket.ans = 0;
                        }
                        if(answerpacket.ans){
                            socketmap[sock] = std::string(buf+2);
                            namemap[std::string(buf+2)] = sock;
                        }
                        std::cout<<"Socket "<<sock<<": "<<answerpacket.loginflag<<" "<<answerpacket.ans;
                        send(sock, (const char*)(&answerpacket), sizeof(answerpacket), 0);
                    }

                }
                else if(buf[0] == 1u){ //msgpckg
                    std::cout<<std::string(buf, sizeof(buf));
                    send(namemap[std::string(buf+34)], buf, sizeof(buf), 0);
                }
            }

        }
    }
}
