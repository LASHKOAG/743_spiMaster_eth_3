#ifndef CREATEPORT_H
#define CREATEPORT_H

//#include "mbed.h"
#include "EthernetInterface.h"
// #include "TCPServer.h"
// #include "TCPSocket.h"

class CreatePort
{
public:
    CreatePort(EthernetInterface , int );
    ~CreatePort();
    TCPSocket srv;  //TCPServer was migrate to TCPSocket
    TCPSocket *clt_sock;
    SocketAddress clt_addr;
    EthernetInterface ptreth;
    //Thread t2;
    int PORT;

    int get_port();
};
#endif // CREATEPORT_H
