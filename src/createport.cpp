#include "createport.h"

CreatePort::CreatePort(EthernetInterface ptreth, int port)
{
    PORT = port;
    flag_AcceptPort=get_port();
    //t2.start();
    
//    TCPSocket srv150;  //TCPServer was migrate to TCPSocket
//    TCPSocket *clt_sock150;
//    SocketAddress clt_addr150;
//    EthernetInterface eth150;
}

int CreatePort::get_port(){
        printf("\n======== step class CreatePort::get_port [%d]======================\n", PORT); fflush(stdout);
        printf("PORT: %d\r\n",PORT);  fflush(stdout);
    srv.open(&ptreth);                         /* Open the server on eth150ernet stack */
    int result_bind = srv.bind(ptreth.get_ip_address(), PORT);     /* Bind the HTTP port (TCP 80) to the server */
        printf("result_bind port[%d] = %d\r\n",PORT, result_bind);
    int result_listen = srv.listen(5);                          /* Can handle 5 simultaneous connections */
        printf("result_listen port[%d] = %d\r\n",PORT, result_listen);

    clt_sock = srv.accept();  //return pointer of a client socket
    int errorAccept = clt_sock->getpeername(&clt_addr);  //this will fill address of client to the SocketAddress object
        printf("\naccept port[%d] %s:%d\n", PORT, clt_addr.get_ip_address(), clt_addr.get_port());

    if(errorAccept != 0 ){
        printf("\nerrorAccept: %d\n", errorAccept);
        return -1;
    }
    return 0;
}

CreatePort::~CreatePort()
{

}
