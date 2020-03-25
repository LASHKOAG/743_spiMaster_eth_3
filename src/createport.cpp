#include "createport.h"

CreatePort::CreatePort()
{
//    TCPSocket srv150;  //TCPServer was migrate to TCPSocket
//    TCPSocket *clt_sock150;
//    SocketAddress clt_addr150;
//    EthernetInterface eth150;
}

int CreatePort::ethernetInterfaceInit()
{
    //EthernetInterface eth150;
    printf("\n======== step EthernetInterfaceFunction() ======================\n");
    fflush(stdout);
    int ret150;
    ret150 = eth150.set_network("192.168.4.177","255.255.255.0","192.168.4.1");   /* set network settings */
    printf("Set Net150: %d\r\n",ret150); fflush(stdout);
 
    if(0 != eth150.connect()) {
            printf("Error connecting \n");
        //return -1;
    }else{
        //eth150.connect();
        printf("\nstep eth150.connect()\n");  fflush(stdout);
        printf("The Server IP 150 address is '%s'\n", eth150.get_ip_address());  fflush(stdout);
        
        srv150.open(&eth150);                         /* Open the server on eth150ernet stack */
        int rrr150 = srv150.bind(eth150.get_ip_address(), 150);     /* Bind the HTTP port (TCP 80) to the server */
        printf("rrr150: %d\r\n",rrr150);
        int rett150 = srv150.listen(5);                          /* Can handle 5 simultaneous connections */
        printf("rett150: %d\r\n",rett150);
        return 0;
    }
    return -2;
}

int CreatePort::ethernetPort(){
    clt_sock150 = srv150.accept();  //return pointer of a client socket
        clt_sock150->getpeername(&clt_addr150);  //this will fill address of client to the SocketAddress object
            printf("\naccept 150 %s:%d\n", clt_addr150.get_ip_address(), clt_addr150.get_port());
         //flagMainThread=true;
         //process_buffer(Recv_msv, 100, clt_sock);
    return 0;
}

CreatePort::~CreatePort()
{

}
