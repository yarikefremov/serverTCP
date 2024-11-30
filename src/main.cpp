#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include "../include/tcpserver.h"

// Need to link with Ws2_32.lib
//IF U USE VC++ compiler
//#pragma comment (lib, "Ws2_32.lib")
//???
// #pragma comment (lib, "Mswsock.lib")
//Also need links to "fwpuclnt.lib", "ntdsapi.lib"


int main(void)
{
    std::string serverIP = "127.0.0.1";

    TCPServer server(serverIP, 54010);

    if (server.initWinsock()) {
        server.run();
    }
    return 0;
}
