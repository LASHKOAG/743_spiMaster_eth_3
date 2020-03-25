#ifndef CREATEPORT_H
#define CREATEPORT_H

//#include "mbed.h"
#include "EthernetInterface.h"
// #include "TCPServer.h"
// #include "TCPSocket.h"

class CreatePort
{
public:
    CreatePort();
    ~CreatePort();
    TCPSocket srv150;  //TCPServer was migrate to TCPSocket
    TCPSocket *clt_sock150;
    SocketAddress clt_addr150;
    EthernetInterface eth150;

    int ethernetInterfaceInit();
    int ethernetPort();
};
#endif // CREATEPORT_H
