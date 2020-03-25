/*
743 spi master eth 3
ports
threads
*/

#include "mbed.h"
#include "platform/mbed_thread.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include <queue>
#include <time.h>
#include <vector>
#include <iostream>
#include "hwclockstm.h"
#include "circbuff.h"
#include <string>
#include "socket_struct.h"
#include <map>    //подключили библиотеку для работы с map
#include <algorithm>
#include "createport.h"

#include "rtos/Thread.h"
//#include "rtos/rtos_idle.h"
#include "platform/mbed_critical.h"

#include "platform/CircularBuffer.h"

#define BUF_SIZE    400
#define MEMP_NUM_NETCONN 8

CircularBuffer<int, BUF_SIZE> buf;
uint32_t bytes_written = 0;

#define DRDY_IN_FLAG (1U << 0)
#define GET_DATA_FROM_SPI_FLAG (1U << 1)
#define PUSH_DATA_TO_ETH_FLAG (1U << 2)
#define READY_TO_SEND_DATA_FLAG (1U << 3)
#define STOP_PUSH_DATA_TO_ETH_FLAG (1U << 4)

// #define PRINT_VECTOR_FLAG (1U << 1)

// #define MAINBUFFER_READY_FROM_SPI_FLAG (1U << 3)
// #define TEST_FLAG (1U << 4)
// #define STOP_GET_DATA_FROM_SPI_FLAG (1U << 5)


#define SIZE_MAINBUFFER 200000
char MainBuffer[SIZE_MAINBUFFER];

char Recv_msv150[100];
char Recv_msvTest[100];
char Recv_msvTest2[100];
char Recv_msvTest3[100];

uint32_t counterDRDY=0;
int32_t counterFLAG=0;
int numberBytesTakenFromSPI=0;
int numberBytesSendToEth=0;
int wasSendAll=0;
int wasSend1=0;
int wasSend2=0;
bool flagGetDataSPI=true;
bool test1=false;
bool flagEndOfMeasurementSPI=true;
int test2=0;
int test3=0;
bool flagMainThread = false;

SPI spi(PD_7, PA_6, PA_5, PA_4); // mosi, miso, sclk, ssel
DigitalOut chipSelect(PA_4);
InterruptIn drdyIN(PE_3);

//Queue myQueue( 1, 5 );

Thread spiThread;
Thread sockSendThread;
Thread resultTaskThread;
Thread portThread150;

// TCPSocket srv, srv150;  //TCPServer was migrate to TCPSocket
// TCPSocket *clt_sock, *clt_sock150;
// SocketAddress clt_addr, clt_addr150;
EthernetInterface eth, eth150;
EventFlags eventFlags;

Thread port80;
Thread port150;
Thread port200;
Thread port300;
Thread port400;
// CriticalSectionLock csLock;


    map <char, char> myMap;
    vector <char> myVec;
    vector<string> myVec2;





int ethernetInterfaceInit()
{
    printf("\n======== step EthernetInterfaceFunction() ======================\n");
    fflush(stdout);
    int ret;
    ret = eth.set_network("192.168.4.177","255.255.255.0","192.168.4.1");   /* set network settings */
    printf("Set Net: %d\r\n",ret); fflush(stdout);
 
    if(0 != eth.connect()) {
            printf("Error connecting \n");
        //return -1;
    }else{
        //eth.connect();
        printf("\nstep eth.connect()\n");  fflush(stdout);
        printf("The Server IP address is '%s'\n", eth.get_ip_address());  fflush(stdout);
        
        //srv.open(&eth); //+                         /* Open the server on ethernet stack */
        // int rrr = srv.bind(eth.get_ip_address(), 80);     /* Bind the HTTP port (TCP 80) to the server */
        // printf("rrr: %d\r\n",rrr);
        // int rett = srv.listen(5);                          /* Can handle 5 simultaneous connections */
        // printf("rett: %d\r\n",rett);

         //srv150.open(&eth);  //+                       /* Open the server on ethernet stack */
        // int rrr120 = srv120.bind(eth.get_ip_address(), 120);     /* Bind the HTTP port (TCP 80) to the server */
        // printf("rrr120: %d\r\n",rrr120);
        // int rett120 = srv120.listen(5);                          /* Can handle 5 simultaneous connections */
        // printf("rett120: %d\r\n",rett120);

        return 0;
    }
    return 0;
}



// void call_port80(){
//         int rrr80 = srv.bind(eth.get_ip_address(), 80);     /* Bind the HTTP port (TCP 80) to the server */
//         printf("rrr80: %d\r\n",rrr80);
//         int rett80 = srv.listen(3);                          /* Can handle 5 simultaneous connections */
//         printf("rett80: %d\r\n",rett80);
//         clt_sock = srv.accept();  //return pointer of a client socket
//         clt_sock->getpeername(&clt_addr);  //this will fill address of client to the SocketAddress object
//             printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
//     while(1){
//         clt_sock->recv(Recv_msvTest, 100);
//             printf("Recv recv_msv %s \n", Recv_msvTest);
//             printf("strlen Recv_msv =%d\n", strlen(Recv_msvTest));
//     }
// }

// void call_port150(){
//         int rrr150 = srv150.bind(eth.get_ip_address(), 150);     /* Bind the HTTP port (TCP 80) to the server */
//             //int rrr150 = srv.bind("192.168.4.177", 150);  
//             printf("rrr150: %d\r\n",rrr150);
//         int rett150 = srv150.listen(3);                          /* Can handle 5 simultaneous connections */
//             printf("rett150: %d\r\n",rett150);
//         clt_sock150 = srv150.accept();  //return pointer of a client socket
//         clt_sock150->getpeername(&clt_addr150);  //this will fill address of client to the SocketAddress object
//             printf("\naccept %s:%d\n", clt_addr150.get_ip_address(), clt_addr150.get_port());
//     while(1){
//         clt_sock150->recv(Recv_msvTest2, 100);
//             printf("Recv recv_msv2 %s \n", Recv_msvTest2);
//             printf("strlen Recv_msv2 =%d\n", strlen(Recv_msvTest2));
//     }
// }

// void call_port200(){
//     CreatePort *port200 = new CreatePort(eth);
//     port200->ethernetInterfaceInit(eth);
//     while(1){
//         port200->clt_sock200->recv(Recv_msvTest3, 100);
//             printf("3 Recv recv_msv3 %s \n", Recv_msvTest3);
//             printf("3 strlen Recv_msv3 =%d\n", strlen(Recv_msvTest3));
//     }
// }

void call_port300(){
    CreatePort *port300 = new CreatePort(eth, 300);
    port300->get_port();
    char *ReceivedMsv = new char[100]{0};
    while(1){
        port300->clt_sock->recv(ReceivedMsv, 100);
            printf("ReceivedMsv port300 = %s \n", ReceivedMsv);
            printf("strlen ReceivedMsv300 = %d\n", strlen(ReceivedMsv));
    }
    delete[] ReceivedMsv;
}

void call_port400(){
    CreatePort *port400 = new CreatePort(eth, 400);
    port400->get_port();
    char *ReceivedMsv = new char[100]{0};
    while(1){
        port400->clt_sock->recv(ReceivedMsv, 100);
            printf("ReceivedMsv port400 = %s \n", ReceivedMsv);
            printf("strlen ReceivedMsv400 = %d\n", strlen(ReceivedMsv));
    }
    delete[] ReceivedMsv;
}
/*
void call_portThread150(){
		CreatePort *port150 = new CreatePort();
        port150->ethernetInterfaceInit();
        port150->ethernetPort();
		while(1){
			port150->clt_sock150->recv(Recv_msv150, 100);
            printf("Recv recv_msv 150 %s \n", Recv_msv150);
            printf("strlen Recv_msv 150 =%d\n", strlen(Recv_msv150));
			//strRecv_msv=Recv_msv;

        //getCommandFromPort3(strRecv_msv);

        //myVec.emplace_back(stoi(strRecv_msv));
        //myVec.emplace_back(0);
		}
}
*/
int main()
{
    printf("\n======== 1-start ======================\n");  fflush(stdout);
    printf("Basic HTTP server example\n");
    //portThread150.start(call_portThread150);
//wait(30);
    spi.format(8,3);        // Setup:  bit data, high steady state clock, 2nd edge capture
    spi.frequency(1000000); //1MHz
    int r;
    int res_put;

    //CircBuffer->initStructCircularBuffer();


    //drdyIN.rise(&drdyINHandlerRise);   //interrupt DRDY flag from slave (hardware)
    //spiThread.start(call_spiThread2);  //get data SPI from Slave equipment
    //sockSendThread.start(call_sockSendThread);  //send data in ethernet port to client
    

    int resEth = ethernetInterfaceInit();

    //port80.start(call_port80);
    //port150.start(call_port150);
    //port200.start(call_port200);
    port300.start(call_port300);
     port400.start(call_port400);

    char Recv_msv[100];                  /* buffer for command from port */
    string strRecv_msv;                  /* buffer for command from port */
    //while(1){
    char Recv_msv120[100];                  /* buffer for command from port */
while(1){}
    // if(resEth==0){
    //     //srv.accept(&clt_sock, &clt_addr);
    //     clt_sock = srv.accept();  //return pointer of a client socket
    //     clt_sock->getpeername(&clt_addr);  //this will fill address of client to the SocketAddress object
    //         printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
    //      flagMainThread=true;
    //      process_buffer(Recv_msv, 100, clt_sock);

    //     clt_sock120 = srv120.accept();  //return pointer of a client socket
    //     clt_sock120->getpeername(&clt_addr120);  //this will fill address of client to the SocketAddress object
    //         printf("\naccept %s:%d\n", clt_addr120.get_ip_address(), clt_addr120.get_port());
    // }
    
    // resultTaskThread.start(call_resultTask, clt_sock);
    
    // while(flagMainThread) {

    //     // srv.accept(&clt_sock, &clt_addr);
    //     //           printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
    //     clt_sock->recv(Recv_msv, 100);
    //         printf("Recv recv_msv %s \n", Recv_msv);
    //         printf("strlen Recv_msv =%d\n", strlen(Recv_msv));
    //                 clt_sock120->recv(Recv_msv120, 100);
    //                 printf("Recv recv_msv120 %s \n", Recv_msv120);
    //     strRecv_msv=Recv_msv;

    //     getCommandFromPort3(strRecv_msv);

    //     myVec.emplace_back(stoi(strRecv_msv));
    //     myVec.emplace_back(0);



    //printf("stoi(9) =%d\n", stoi("9"));
    
    // myVec2.emplace_back(stoi(strRecv_msv));
    // myVec2.emplace_back(0);
    // printf("stoi(9) =%d\n", stoi("9"));


    // for (auto it = myMap.begin(); it != myMap.end(); ++it)
    // {
    //     cout << it->first << " : " << it->second << endl;
    // }
    // cout << " =========== " << endl;

    // for (auto it = myVec.begin(); it != myVec.end(); ++it)
    // {
    //     cout << *it << endl;
    // }
    // cout << " =========== " << endl;

    // findInVector(myVec, (char)43);
    // cout << findInVector(myVec, (char)43).first << endl;
    // cout << findInVector(myVec, (char)43).second << endl;
    // cout << " =========== " << endl;
    //}
    //}
}

/*
  map <string,int> myFirstMap = {{ "Mother", 37 },
                                 { "Father", 40 },///map явно инициализирована
                                 { "Brother", 15 },
                                 { "Sister", 20 }};
 
  ///вывод явно инициализированной map на экран
  for (auto it = myFirstMap.begin(); it != myFirstMap.end(); ++it)
  {
      cout << it->first << " : " << it->second << endl;
  }
*/
/*

        queue<string> myQueue;     // создаем пустую очередь типа string
 
    // добавили в очередь несколько элементов типа string
    myQueue.push("No pain ");
    myQueue.push("- no gain");
 
    cout << "Количество элементов в очереди: " << myQueue.size() << endl;
    cout << "\nВот они: " << myQueue.front() << myQueue.back();
 
    myQueue.pop(); // удаляем один элемент в очереди
    cout << "\nТеперь в очереди остался один элемент: " << myQueue.front();
    cout << "Количество элементов в очереди: " << myQueue.size() << endl;
*/
//apArray->goToStartWriteIndex=true;
// apArray->goToStartReadIndex=true;

/*
#define HTTP_STATUS_LINE "HTTP/1.0 200 OK"
#define HTTP_HEADER_FIELDS "Content-Type: text/html; charset=utf-8"
#define HTTP_MESSAGE_BODY ""                                     \
"<html>" "\r\n"                                                  \
"  <body style=\"display:flex;text-align:center\">" "\r\n"       \
"    <div style=\"margin:auto\">" "\r\n"                         \
"      <h1>Hello World</h1>" "\r\n"                              \
"      <p>It works !</p>" "\r\n"                                 \
"    </div>" "\r\n"                                              \
"  </body>" "\r\n"                                               \
"</html>"

#define HTTP_RESPONSE HTTP_STATUS_LINE "\r\n"   \
                      HTTP_HEADER_FIELDS "\r\n" \
                      "\r\n"                    \
                      HTTP_MESSAGE_BODY "\r\n"
*/

//clt_sock.send(HTTP_RESPONSE, strlen(HTTP_RESPONSE));
//clt_sock.send("check sinBuffer", strlen("check sinBuffer"));

/*
void call_spiThread(void const *args){ //working
  structBuffer* rcvArgs=(structBuffer*)args;
  uint32_t read_flags = 0;
  uint32_t count=0;
        char txBufferMsv[3]={0, };
        char rxBufferMsv[3]={0, };
  while(1){
    //char * txBufferMsv = new char[3];
    //char * rxBufferMsv = new char[3];

    //read_flags = eventFlags.wait_any(BUTTON_PRESSED_FLAG); //1)ждем один флаг
    read_flags = eventFlags.wait_all(DRDY_IN_FLAG); //ждем флаг

    chipSelect = 0; // Select device
    spi.write(txBufferMsv, 3, rxBufferMsv, 3);
    chipSelect = 1; // Deselect device

//for test master get reference samples from slave
    // for(uint8_t i=0; i<3; ++i){
    //     printf("rxBufferMsv[%d] = %d \n", i, rxBufferMsv[i]);
    // }

    for(uint32_t i=0; i<3; ++i, ++count){
      rcvArgs->MainBuffer[count] = rxBufferMsv[i];
    }

    if(count==sizeMainBuffer){
        // for(uint32_t i=0; i<sizeMainBuffer; ++i){
        //   printf("rcvArgs->MainBuffer[%d] = %d \n", i, rcvArgs->MainBuffer[i]);
        // }
        count=0;
        eventFlags.set(PRINT_VECTOR_FLAG);
    }
    //delete[] txBufferMsv;
    //delete[] rxBufferMsv;
  }
}
*/


//std::vector<char> VectorBytes(1024);
//std::vector<char>::iterator itVectorBytes;
//std::vector<double> Buffer(sizeptrBuffer);
//std::vector<int8_t> BufferBytes(1000);
//std::vector<int8_t>::iterator itBufferBytes;
//char massive[] = "123456789012345678901234567890";


//spiThread.start(call_spiThread, &structBuff);
//spiThread.start(call_spiThread2);

/*
if(eventFlags.wait_all(PRINT_VECTOR_FLAG)){
    for(int i=0; i<sizeMainBuffer; ++i){
      BufferBytes.push_back(structBuff.MainBuffer[i]);
    }
    for (itBufferBytes = BufferBytes.begin(); itBufferBytes != BufferBytes.end(); ++itBufferBytes){
        std::cout << "itBufferBytes=" << *itBufferBytes << std::endl;
    }
}
*/



// for(int i=0; i<sizeMainBuffer; ++i){
//    printf("MainBuffer[%d] = %d \n", i, MainBuffer[i]);
// }


// for(int i=0; i<sizeMainBuffer; ++i){
//    printf("structBuff.MainBuffer[%d] = %d \n", i, structBuff.MainBuffer[i]);
// }

// for(int i=0; i<sizeMainBuffer; ++i){
//   structBuff.MainBuffer[i]=0;
// }

/*
Timeout to1;

class Counter {
public:
    Counter(PinName pin) : _interrupt(pin) {        // create the InterruptIn on the pin specified to Counter
        _interrupt.rise(this, &Counter::increment); // attach increment function of this counter instance
    }

    void increment() {
        _count++;
    }

    int read() {
        return _count;
    }

        void print(){
          printf("Count so far: %d\n", read());
          //_count=0;
        }

private:
    InterruptIn _interrupt;
    volatile int _count;
};
Counter counter(PE_3);
*/

/*
void call_printerThread(uint32_t* ptrCnt){
  uint32_t read_flags3 = 0;
  while(1){
    //read_flags = eventFlags.wait_any(BUTTON_PRESSED_FLAG); //1)ждем один флаг
    //read_flags = eventFlags.wait_all(DRDY_IN_FLAG); //ждем флаг
    read_flags3 = eventFlags.wait_all(DRDY_100_FLAG); //ждем флаг
    ptrCnt++;
    //printf("HELLO ARM\n");
  }
}
*/

/*
void call_counterThread(){
  uint32_t read_flags2 = 0;
  uint32_t counter_flags = 0;
  while(1){
    //read_flags = eventFlags.wait_any(BUTTON_PRESSED_FLAG); //1)ждем один флаг
    read_flags2 = eventFlags.wait_all(DRDY_100_FLAG); //ждем флаг
    counter_flags++;
    printf("counter_flags = %u \n", counter_flags);
  }
}
*/
