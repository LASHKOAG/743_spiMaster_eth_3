/*
743 spi master eth 3
testClock
staticMSV ring read-write in parallel
*/

#include "mbed.h"
#include "platform/mbed_thread.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include <time.h>
#include <vector>
#include <iostream>
#include "hwclockstm.h"
#include "circbuff.h"

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

CircBuff *CircBuffer= new CircBuff();
//CircBuffer->initStructCircularBuffer();
// struct structBuffer{
//     int8_t MainBuffer[sizeMainBuffer];
// }structBuff;

int8_t onOffLed()                                             // function for check
{
    DigitalOut led3(LED3);
    for(int i=0; i<100; ++i) {
        led3 = !led3;
        wait_ms(100);
    }
    return 0;
}


void drdyINHandlerRise()                                        //set flag after interrupt drdy
{
    eventFlags.set(DRDY_IN_FLAG);
}

void call_sockSendThread()
{
    int sizeTempBuf=100;
    char TempBuf[sizeTempBuf];
    int counter=0;
    bool flagSendFirstPartToEth=true;
    int step=0;

    while(1) {
        if(eventFlags.wait_all(PUSH_DATA_TO_ETH_FLAG | READY_TO_SEND_DATA_FLAG, osWaitForever, false)) {
            step=1;
        }

        switch (step) {
            case 1:
                while(flagSendFirstPartToEth) {
                    for(counter; counter<sizeTempBuf; ) {
                        if(flagEndOfMeasurementSPI==false) {
                            flagSendFirstPartToEth=false;
                            step=2;
                            break;
                        }
                        TempBuf[counter]=CircBuffer->getCircularBuffer(MainBuffer, SIZE_MAINBUFFER);
                        numberBytesSendToEth++;
                        counter++;
                    }
                    wasSend1+=clt_sock.send(&TempBuf[0], counter);
                    counter=0;
                }
            //break;

            case 2:
                counter=0;
                int difference=(numberBytesTakenFromSPI - numberBytesSendToEth);
                if(difference==0) {
                    break;
                }
                //while(difference--){
                while(difference) {
                    for(counter; counter<sizeTempBuf; ) {
                        if(difference==0) {
                            break;
                        }
                        TempBuf[counter]=CircBuffer->getCircularBuffer(MainBuffer, SIZE_MAINBUFFER);
                        numberBytesSendToEth++;
                        difference=(numberBytesTakenFromSPI - numberBytesSendToEth);
                        counter++;
                    }

                    wasSend2+=clt_sock.send(&TempBuf[0], counter);
                    counter=0;
                }
                counter=0;
                wasSend1=0;/*для отладки*/
                wasSend2=0;/*для отладки*/
                step=0;
                printf("numberBytesTakenFromSPI %d \n",numberBytesTakenFromSPI);fflush(stdout);
                numberBytesSendToEth=0;
                numberBytesTakenFromSPI=0;
                flagSendFirstPartToEth=true;
                CircBuffer->clearCircularBuffer();  /*переставляем индексы кольцевого буффера в начало*/
                //eventFlags.clear(STOP_PUSH_DATA_TO_ETH_FLAG);
                break;
        }
    }
}


void call_spiThread2()
{
    uint32_t read_flags = 0;
    uint32_t count=0;
    bool flagEnableGetSPI=true;  /*опрашивать SPI или нет*/
    char txBufferMsv[3]= {0, };
    char rxBufferMsv[3]= {0, };

    while(eventFlags.wait_all(GET_DATA_FROM_SPI_FLAG, osWaitForever, false)) {
        flagEnableGetSPI=true;/* опрашивать SPI */
        //if(eventFlags.wait_all(STOP_GET_DATA_FROM_SPI_FLAG, osWaitForever, false)){} //flagGetDataSPI=false;
        //read_flags = eventFlags.wait_any(BUTTON_PRESSED_FLAG); //1)ждем один флаг
        read_flags = eventFlags.wait_all(DRDY_IN_FLAG | GET_DATA_FROM_SPI_FLAG, osWaitForever, false); //waiting for all flags
        //printf("count = %u\n", count);
        chipSelect = 0; // Select device
        spi.write(txBufferMsv, 3, rxBufferMsv, 3);
        chipSelect = 1; // Deselect device
        //for test master get reference samples from slave
        // for(uint8_t i=0; i<3; ++i){
        //     printf("rxBufferMsv[%d] = %d \n", i, rxBufferMsv[i]);
        // }
        if(flagGetDataSPI==false) {
            /*Сигнал о прекращении опроса по SPI может поступить
             как в цикле for (ниже), так и до наступления момента входа в этот цикл.
             Без этого куска кода в массив прибавится Байт*/
            numberBytesTakenFromSPI=count;
            count=0;  //printf("STEP 2 \n");fflush(stdout);
            eventFlags.clear(GET_DATA_FROM_SPI_FLAG);
            //eventFlags.set(MAINBUFFER_READY_FROM_SPI_FLAG);  //printf("STEP 3 \n");fflush(stdout);
            flagEnableGetSPI =false; /*опрашивать SPI или нет*/
        }

        if(flagEnableGetSPI) {
            for(uint8_t i=0; i<3; i++) { //24bit ADC
                //VectorBytes.push_back(rxBufferMsv[i]);
                CircBuffer->putCircularBuffer(MainBuffer, SIZE_MAINBUFFER, rxBufferMsv[i]);
                if(i==1) {
                    eventFlags.set(READY_TO_SEND_DATA_FLAG);   /*чтобы READ из буфера в порт не стартовал раньше WRITE в буфер данных*/
                }

                count++;        /*пояснение к абзацу выше: здесь может в массив прибавится тот самый Байт*/
                if(flagGetDataSPI==false) {
                    numberBytesTakenFromSPI=count;
                    count=0;  //printf("STEP 2 \n");fflush(stdout);
                    eventFlags.clear(GET_DATA_FROM_SPI_FLAG);
                    //eventFlags.set(MAINBUFFER_READY_FROM_SPI_FLAG);  //printf("STEP 3 \n");fflush(stdout);
                    break;
                }
            }
        }
    }
}

void ethernetInterfaceInit()
{
    //EthernetInterface eth;
    printf("\n======== step EthernetInterfaceFunction() ======================\n");
    fflush(stdout);
    int ret;
    ret = eth.set_network("192.168.4.177","255.255.255.0","192.168.4.1");   /* set network settings */
    printf("Set Net: %d\r\n",ret);
    fflush(stdout);

    eth.connect();
    printf("\nstep eth.connect()\n");
    fflush(stdout);
    printf("The target IP address is '%s'\n", eth.get_ip_address());
    fflush(stdout);

    srv.open(&eth);                         /* Open the server on ethernet stack */
    srv.bind(eth.get_ip_address(), 80);     /* Bind the HTTP port (TCP 80) to the server */
    srv.listen(5);                          /* Can handle 5 simultaneous connections */
}

void getCommandFromPort(char* ptr_recv_msv)
{
    int32_t valueFromCommand = ptr_recv_msv[0]-0x30;
    /* обнулить входящий массив после того как он отработает*/

    switch (valueFromCommand) {
        case 2:
            printf("\nget command from port: %d\n", valueFromCommand);
            fflush(stdout);
            eventFlags.set(GET_DATA_FROM_SPI_FLAG);
            eventFlags.set(PUSH_DATA_TO_ETH_FLAG);
            flagGetDataSPI=true;
            flagEndOfMeasurementSPI=true;
            break;
        case 3:
            printf("\nget command from port: %d \n", valueFromCommand);
            fflush(stdout);
            printf("\n onOffLed() function has to work!\n");
            fflush(stdout);
            onOffLed();
            break;
        case 4:
            printf("\nget command %d from port\n", valueFromCommand);
            fflush(stdout);
            flagEndOfMeasurementSPI=false;
            //eventFlags.set(STOP_PUSH_DATA_TO_ETH_FLAG);
            eventFlags.clear(PUSH_DATA_TO_ETH_FLAG);
            eventFlags.clear(READY_TO_SEND_DATA_FLAG);
            flagGetDataSPI=false;
            //printf("buff->readIndex=%d \n",buff.readIndex);fflush(stdout);
            //printf("buff->writeIndex=%d \n",buff.writeIndex);fflush(stdout);
            //printf("numberBytesTakenFromSPI=%d \n",numberBytesTakenFromSPI);
            fflush(stdout);
            //printf("bool test1=%d \n",test1);fflush(stdout);
            printf("============================ \n");
            fflush(stdout);
            //printf("numberBytesSendToEth %d \n",numberBytesSendToEth);fflush(stdout);
            //printf("numberBytesTakenFromSPI %d \n",numberBytesTakenFromSPI);fflush(stdout);
            //printf("wasSendAll=%d \n",wasSendAll);fflush(stdout);
            break;
        case 5:
            printf("\nget command %d from port\n", valueFromCommand);
            fflush(stdout);
            printf("\n port close()\n");
            fflush(stdout);


            // srv.close();
            // //eth.
            // eth.disconnect();
            // flag1=false;
            // wait(120);
            break;
    }
}

int main()
{
    printf("\n======== 1-start ======================\n");
    fflush(stdout);
    
    //CircBuff *CircBuffer= new CircBuff();
    
    printf("Basic HTTP server example\n");

    spi.format(8,3);        // Setup:  bit data, high steady state clock, 2nd edge capture
    spi.frequency(1000000); //1MHz
    int r;
    int res_put;

    //CircBuffer->initStructCircularBuffer();


    drdyIN.rise(&drdyINHandlerRise);   //interrupt DRDY flag from slave (hardware)
    spiThread.start(call_spiThread2);  //get data SPI from Slave equipment
    sockSendThread.start(call_sockSendThread);  //send data in ethernet port to client

    ethernetInterfaceInit();

    char Recv_msv[100];                  /* buffer for command from port */
    //while(1){
    srv.accept(&clt_sock, &clt_addr);
    printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
    // flag1=true;
    while(1) {

        // srv.accept(&clt_sock, &clt_addr);
        //           printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
        clt_sock.recv(Recv_msv, 100);
        printf("Recv recv_msv %s \n", Recv_msv);
        getCommandFromPort(&Recv_msv[0]);
    }
    //}
}

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