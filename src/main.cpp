/*
743 spi master eth 3v4
test vector
*/

#include "mbed.h"
#include "platform/mbed_thread.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include <time.h>
#include <vector>
#include <iostream>

#define DRDY_IN_FLAG (1U << 0)
#define PRINT_VECTOR_FLAG (1U << 1)
#define GET_DATA_FROM_SPI_FLAG (1U << 2)
#define MAINBUFFER_READY_FROM_SPI_FLAG (1U << 3)
#define TEST_FLAG (1U << 4)
#define STOP_GET_DATA_FROM_SPI_FLAG (1U << 5)

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


SPI spi(PD_7, PA_6, PA_5, PA_4); // mosi, miso, sclk, ssel
DigitalOut chipSelect(PA_4);
InterruptIn drdyIN(PE_3);

Thread spiThread;
Thread sockSendThread;

TCPServer srv;
TCPSocket clt_sock;
SocketAddress clt_addr;
EthernetInterface eth;

EventFlags eventFlags;

bool flagGetDataSPI=true;
const uint32_t sizeMainBuffer=8192;
//uint32_t sizeMainBuffer=10;

uint32_t counterDRDY=0;

int32_t counterFLAG=0;

//std::vector<char> VectorBytes(1024);
std::vector<char> VectorBytes;
std::vector<char>::iterator itVectorBytes;

// struct structBuffer{
//     int8_t MainBuffer[sizeMainBuffer];
// }structBuff;


int8_t onOffLed(){                                            // function for check
    DigitalOut led3(LED3);
    for(int i=0; i<100; ++i){
        led3 = !led3;
        wait_ms(100);
    }
    return 0;
}


void drdyINHandlerRise(){                                       //set flag after interrupt drdy
  eventFlags.set(DRDY_IN_FLAG);
}

void call_sockSendThread(){                                     /*send in port MainBuffer - if MainBuffer is ready*/
    while(eventFlags.wait_all(MAINBUFFER_READY_FROM_SPI_FLAG)){
        clt_sock.send(&VectorBytes[0], VectorBytes.size());
        // for (int i = 0; i < sizeMainBuffer; i++) {
        //     printf("MainBuffer[%d] = %x\n", i, MainBuffer[i]);
        // }
        printf("MainBuffer was send, VectorBytes.size()=%u \n", VectorBytes.size());fflush(stdout);
        //VectorBytes.clear();
    }
}


void call_spiThread2(){
  uint32_t read_flags = 0;
  uint32_t count=0;
  bool flagEnableGetSPI=true;  /*опрашивать SPI или нет*/
  char txBufferMsv[3]={0, };
  char rxBufferMsv[3]={0, };

  while(eventFlags.wait_all(GET_DATA_FROM_SPI_FLAG, osWaitForever, false)){
    flagEnableGetSPI=true;/* опрашивать SPI */
    //if(eventFlags.wait_all(STOP_GET_DATA_FROM_SPI_FLAG, osWaitForever, false)){} //flagGetDataSPI=false;
    //read_flags = eventFlags.wait_any(BUTTON_PRESSED_FLAG); //1)ждем один флаг
    read_flags = eventFlags.wait_all(DRDY_IN_FLAG | GET_DATA_FROM_SPI_FLAG, osWaitForever, false); //waiting for all flags
        printf("count = %u\n", count);
    chipSelect = 0; // Select device
    spi.write(txBufferMsv, 3, rxBufferMsv, 3);
    chipSelect = 1; // Deselect device
                                    //for test master get reference samples from slave
                                        // for(uint8_t i=0; i<3; ++i){
                                        //     printf("rxBufferMsv[%d] = %d \n", i, rxBufferMsv[i]);
                                        // }
    if(count==sizeMainBuffer || flagGetDataSPI==false){
      /*Сигнал о прекращении опроса по SPI может поступить
       как в цикле for (ниже), так и до наступления момента входа в этот цикл.
       Без этого куска кода в массив прибавится Байт*/
        count=0;  //printf("STEP 2 \n");fflush(stdout);
        eventFlags.clear(GET_DATA_FROM_SPI_FLAG);
        eventFlags.set(MAINBUFFER_READY_FROM_SPI_FLAG);  //printf("STEP 3 \n");fflush(stdout);
        flagEnableGetSPI =false; /*опрашивать SPI или нет*/
    }

    if(flagEnableGetSPI){
        for(uint8_t i=0; i<3; i++){ //24bit ADC
            VectorBytes.push_back(rxBufferMsv[i]);
            count++;        /*пояснение к абзацу выше: здесь может в массив прибавится тот самый Байт*/
            if(count==sizeMainBuffer || flagGetDataSPI==false){
                count=0;  //printf("STEP 2 \n");fflush(stdout);
                eventFlags.clear(GET_DATA_FROM_SPI_FLAG);
                eventFlags.set(MAINBUFFER_READY_FROM_SPI_FLAG);  //printf("STEP 3 \n");fflush(stdout);
                break;
                //wait(5);
            }
        }
    }
  }
}

void ethernetInterfaceInit(){
    //EthernetInterface eth;
            printf("\n======== step EthernetInterfaceFunction() ======================\n");fflush(stdout);
    int ret;
    ret = eth.set_network("192.168.4.177","255.255.255.0","192.168.4.1");   /* set network settings */
            printf("Set Net: %d\r\n",ret);fflush(stdout);
    
    eth.connect();
            printf("\nstep eth.connect()\n");fflush(stdout);
            printf("The target IP address is '%s'\n", eth.get_ip_address());fflush(stdout);
    
    srv.open(&eth);                         /* Open the server on ethernet stack */
    srv.bind(eth.get_ip_address(), 80);     /* Bind the HTTP port (TCP 80) to the server */
    srv.listen(5);                          /* Can handle 5 simultaneous connections */
}

void getCommandFromPort(char* ptr_recv_msv){
  int32_t valueFromCommand = ptr_recv_msv[0]-0x30;
/* обнулить входящий массив после того как он отработает*/
  switch (valueFromCommand){
    case 2:
        printf("\nget command %d from port\n", valueFromCommand);fflush(stdout);
        eventFlags.set(GET_DATA_FROM_SPI_FLAG);
        //eventFlags.set(TEST_FLAG);
        flagGetDataSPI=true;
      break;
    case 3:
        printf("\nget command %d from port\n", valueFromCommand);fflush(stdout);
        printf("\n onOffLed() function has to work!\n");fflush(stdout);
        onOffLed();
      break;
    case 4:
        printf("\nget command %d from port\n", valueFromCommand);fflush(stdout);
        flagGetDataSPI=false;
      break;
    case 5:
        printf("\nget command %d from port\n", valueFromCommand);fflush(stdout);
        printf("\n port close()\n");fflush(stdout);
        // srv.close();
        // //eth.
        // eth.disconnect();
        // flag1=false;
        // wait(120); 
      break;
  }
}

int main() {
        printf("\n======== 1-start ======================\n");fflush(stdout);
        printf("Basic HTTP server example\n");
    
    spi.format(8,3);        // Setup:  bit data, high steady state clock, 2nd edge capture
    spi.frequency(1000000); //1MHz

    VectorBytes.push_back(0x10);
    VectorBytes.push_back(0x20);
    VectorBytes.push_back(0x30);
    VectorBytes.push_back(0x40);

    drdyIN.rise(&drdyINHandlerRise);   //interrupt DRDY flag from slave (hardware)
    spiThread.start(call_spiThread2);  //get data SPI from Slave equipment
    sockSendThread.start(call_sockSendThread);  //send data in ethernet port to client

    ethernetInterfaceInit();

    char Recv_msv[100];                  /* buffer for command from port */
    //while(1){
    srv.accept(&clt_sock, &clt_addr);
            printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
    // flag1=true;
    while(1){
      
      // srv.accept(&clt_sock, &clt_addr);
      //           printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
      clt_sock.recv(Recv_msv, 100);
                printf("Recv recv_msv %s \n", Recv_msv);
      
      getCommandFromPort(&Recv_msv[0]);
    }
    //}
}

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