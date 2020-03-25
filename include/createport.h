#ifndef CREATEPORT_H
#define CREATEPORT_H

//#include "mbed.h"
#include "EthernetInterface.h"
// #include "TCPServer.h"
// #include "TCPSocket.h"

class CreatePort
{
public:
    CreatePort(EthernetInterface );
    ~CreatePort();
    TCPSocket srv200;  //TCPServer was migrate to TCPSocket
    TCPSocket *clt_sock200;
    SocketAddress clt_addr200;
    EthernetInterface eth200;

    int ethernetInterfaceInit(EthernetInterface ptreth);
    int ethernetPort();
};
#endif // CREATEPORT_H
