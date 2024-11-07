#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "../include/tcpserver.h"
#include "../include/passchecker.h"

//NO VC++ only clear C++
// Need to link with Ws2_32.lib
//IF U USE VC++ compiler
//#pragma comment (lib, "Ws2_32.lib")
//???
// #pragma comment (lib, "Mswsock.lib")
//Also need links to "fwpuclnt.lib", "ntdsapi.lib"

#define DEFAULT_BUFLEN 512 //#define RECV_DATA_BUF_SIZE 256
#define DEFAULT_PORT "27015"


int main(void)
{

    // Set the exclusive address option
    /*char iOptval = 1;
    iResult = setsockopt(ListenSocket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                         (char *) &iOptval, sizeof (iOptval));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"setsockopt for SO_EXCLUSIVEADDRUSE failed with error: %ld\n",
                WSAGetLastError());
    }*/

    Passchecker passchecker;

    std::string serverIP = "127.0.0.1";

    TCPServer server(serverIP, 54010);

    //Debug
    passchecker.userRegister("Meepo", "123456");
    passchecker.isNewUser("Meepo");
    if (server.initWinsock()) {
        server.run();
    }
}
