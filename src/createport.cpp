#include "createport.h"

CreatePort::CreatePort(EthernetInterface ptreth)
{
//    TCPSocket srv150;  //TCPServer was migrate to TCPSocket
//    TCPSocket *clt_sock150;
//    SocketAddress clt_addr150;
//    EthernetInterface eth150;
}

int CreatePort::ethernetInterfaceInit(EthernetInterface ptreth)
{
    printf("\n======== step class EthernetInterfaceFunction() ======================\n");
    fflush(stdout);
    // int ret200;
    // ret200 = ptreth.set_network("192.168.4.177","255.255.255.0","192.168.4.1");   /* set network settings */
    // printf("200 Set Net: %d\r\n",ret200); fflush(stdout);
 
    // if(0 != eth.connect()) {
    //         printf("Error connecting \n");
    //     //return -1;
    // }else{
        //eth150.connect();
        printf("\nstep eth200.connect()\n");  fflush(stdout);
        printf("The Server IP 200 address is '%s'\n", ptreth.get_ip_address());  fflush(stdout);
        
        srv200.open(&ptreth);                         /* Open the server on eth150ernet stack */
        int rrr200 = srv200.bind(ptreth.get_ip_address(), 200);     /* Bind the HTTP port (TCP 80) to the server */
        printf("rrr200: %d\r\n",rrr200);
        int rett200 = srv200.listen(5);                          /* Can handle 5 simultaneous connections */
        printf("rett200: %d\r\n",rett200);

            clt_sock200 = srv200.accept();  //return pointer of a client socket
        clt_sock200->getpeername(&clt_addr200);  //this will fill address of client to the SocketAddress object
            printf("\naccept 200 %s:%d\n", clt_addr200.get_ip_address(), clt_addr200.get_port());
        //return 0;
    //}
    return -2;
}

int CreatePort::ethernetPort(){
    clt_sock200 = srv200.accept();  //return pointer of a client socket
        clt_sock200->getpeername(&clt_addr200);  //this will fill address of client to the SocketAddress object
            printf("\naccept 200 %s:%d\n", clt_addr200.get_ip_address(), clt_addr200.get_port());
         //flagMainThread=true;
         //process_buffer(Recv_msv, 100, clt_sock);
    return 0;
}

CreatePort::~CreatePort()
{

}
