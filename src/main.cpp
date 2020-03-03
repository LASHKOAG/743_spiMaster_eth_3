/*
743 spi master eth 3v4
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

int countWrittenElements=0;
int wasSend=0;
int wasSend2=0;  
int wasSend3=0;  
bool test1=false;
int test2=0;
int test3=0;

typedef struct
{
  int readIndex;
  int writeIndex;
  bool isEmpty;
  bool isFull;
  bool goToStartWriteIndex;
  bool goToStartReadIndex;
  //char data[BUFFER_SIZE_DATA_FROM_SPI];
}sCircularBuffer;

sCircularBuffer buff;

void init(sCircularBuffer *apArray)
{
  apArray->readIndex  = 0;
  apArray->writeIndex = 0;
  apArray->isEmpty    = 1;
  apArray->isFull     = 0;
}
//------------------------------------------------------------------------------
int put(sCircularBuffer *apArray, char* ptrMainBuffer, int aValue)
{
  if(apArray->isFull){return -1;} /*если буффер полон*/
  if(apArray->writeIndex >= SIZE_MAINBUFFER){apArray->writeIndex = 0;test3++;} /*переместиться в самое начало*/
  if(apArray->isEmpty) /*если буффер пуст*/
  {
    apArray->isEmpty = 0;
    // apArray->data[apArray->writeIndex++] = aValue;
    ptrMainBuffer[apArray->writeIndex++] = aValue;

    if (apArray->writeIndex == apArray->readIndex)/*один индекс догнал другой, буффер полон*/
      apArray->isFull  = 1;
    return 1;
  }

  ptrMainBuffer[apArray->writeIndex++] = aValue; /*если буффер не пуст*/

  if (apArray->writeIndex == apArray->readIndex){apArray->isFull  = 1;}

  return 1;
}
//------------------------------------------------------------------------------
char get(sCircularBuffer *apArray, char* ptrMainBuffer)
{
  if(apArray->isEmpty){return -1;}  /*если буффер пуст*/
  
  apArray->isFull = 0;  /*??? if ?? если буффер не пуст*/

  if(apArray->readIndex >= SIZE_MAINBUFFER){apArray->readIndex = 0;} 
 
  char res = ptrMainBuffer[apArray->readIndex++];
  //while((apArray->readIndex + 1) == apArray->writeIndex){;}
  
if((apArray->readIndex + 1) == apArray->writeIndex){
  /*В самом начале скорость чтения больше скорости записи буффера.
   Это приведет к ошибочному сбору данных.
   Данная проверка "притормаживает" read из буффера.*/
    test1=true;
    wait_us(7000);
    // printf("bool test1=%d \n",test1);fflush(stdout);
    // test1=false;
    // for(uint32_t i =0; i<1000000; ++i){;}
}
//if((apArray->readIndex % 10000)==0){test2++;}
//if(test2==2){printf("\nstep STOP \n");fflush(stdout);}
  if(apArray->readIndex == apArray->writeIndex){apArray->isEmpty = 1;}
  
  return  res;
}
//------------------------------------------------------------------------------
void clear(sCircularBuffer *apArray)
{
  apArray->isEmpty    = 1;
  apArray->isFull     = 0;
  apArray->writeIndex = 0;
  apArray->readIndex  = 0;
}
//------------------------------------------------------------------------------
int isEmpty(sCircularBuffer *apArray)
{
  return apArray->isEmpty;
}
//------------------------------------------------------------------------------
int isFull(sCircularBuffer *apArray)
{
  return apArray->isFull;
}

//------------------------------------------------------------------------------

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
//const uint32_t sizeMainBuffer=8192;
//uint32_t sizeMainBuffer=10;

uint32_t counterDRDY=0;
int32_t counterFLAG=0;

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

void call_sockSendThread(){ 
    int sizeTempBuf=100;   
    char TempBuf[sizeTempBuf];
    bool waitFlags2=false;
    bool waitFlags=true;
    bool test6=true;
    int globalCountTempBuf=0;
    bool onWhile=false;
    
    while(eventFlags.wait_all(PUSH_DATA_TO_ETH_FLAG | READY_TO_SEND_DATA_FLAG, osWaitForever, false)){
        for(int i=0; i<sizeTempBuf; ++i){
           TempBuf[i]=get(&buff, MainBuffer);
        }
          wasSend2=clt_sock.send(&TempBuf[0], sizeTempBuf);
          wasSend3+=wasSend2;
    }
/*
    while(waitFlags){
            if(eventFlags.wait_all(PUSH_DATA_TO_ETH_FLAG | READY_TO_SEND_DATA_FLAG, osWaitForever, false)){
            waitFlags2=true;
            waitFlags=false;
            //printf("step1 \n");fflush(stdout);
        }
    }
    //while(eventFlags.wait_all(PUSH_DATA_TO_ETH_FLAG | READY_TO_SEND_DATA_FLAG, osWaitForever, false)){
      
      while(waitFlags2){
        //printf("step2 \n");fflush(stdout);
        eventFlags.wait_all(PUSH_DATA_TO_ETH_FLAG | READY_TO_SEND_DATA_FLAG, osWaitForever, false);
        //printf("step3 \n");fflush(stdout);
if(eventFlags.wait_all(STOP_PUSH_DATA_TO_ETH_FLAG, osWaitForever, false)){
           for(int i=0; i<sizeTempBuf; ++i){
           TempBuf[i]=get(&buff, MainBuffer);
           if(buff.readIndex==buff.writeIndex){
             waitFlags2=false;
             waitFlags=true;
             wasSend2=clt_sock.send(&TempBuf[0], i);
             wasSend3+=wasSend2;
              break;
             }
           //wasSend++;
           //if(wasSend >= buff.readIndex){break;}
         }
}else{
          for(int i=0; i<sizeTempBuf; ++i){
           TempBuf[i]=get(&buff, MainBuffer);

           //wasSend++;
           //if(wasSend >= buff.readIndex){break;}
         }
          wasSend2=clt_sock.send(&TempBuf[0], sizeTempBuf);
          wasSend3+=wasSend2;
}
         //printf("step1 \n");fflush(stdout);
         //wait_ms(1);
         //if(buff.readIndex==buff.writeIndex){test6=false;}
         for(int i=0; i<sizeTempBuf; ++i){
           TempBuf[i]=get(&buff, MainBuffer);
           if(buff.readIndex==buff.writeIndex){
             test6=false;globalCountTempBuf=i;
             wasSend2=clt_sock.send(&TempBuf[0], i);
             wasSend3+=wasSend2;
              break;
             }
           //wasSend++;
           //if(wasSend >= buff.readIndex){break;}
         }
          wasSend2=clt_sock.send(&TempBuf[0], sizeTempBuf);
          wasSend3+=wasSend2;
*/
          if(wasSend3 >= SIZE_MAINBUFFER){wasSend3=0;}
         //char sendByte = get(&buff, MainBuffer);
        //  clt_sock.send(&sendByte, 1);
        /*
        if(wasSend >= buff.readIndex){
          wasSend2=clt_sock.send(&TempBuf[0], wasSend);
          wasSend3+=wasSend2;
        }else{
          wasSend2=clt_sock.send(&TempBuf[0], sizeTempBuf);
          wasSend3+=wasSend2;
        }
        */
        //wasSend+=sizeTempBuf;
      //for(int i=0; i<BUFFER_SIZE_DATA_FROM_SPI; ++i){printf("DataFromSPI[%d] = %d\n", i, DataFromSPI[i]);fflush(stdout);}
      //  printf("buff->readIndex=%d \n",buff.readIndex);fflush(stdout);
      //  printf("buff->writeIndex=%d \n",buff.writeIndex);fflush(stdout);
      // printf("buff.isEmpty=%d \n",buff.isEmpty);fflush(stdout);
      // printf("buff.isFull=%d \n",buff.isFull);fflush(stdout);
      // printf("buff.writeIndex-buff.readIndexl=%d \n",buff.writeIndex-buff.readIndex);fflush(stdout);
      //BUFFER_SIZE_DATA_FROM_SPI - buff.writeIndex(от места начала записи) +buff.writeIndex(текущий)
      //char result = get(&buff, DataFromSPI);
// int countDataToEth = buff.writeIndex-buff.readIndex;
// printf("countDataToEth=%d \n",countDataToEth);fflush(stdout);
// printf("countWrittenElements=%d \n",countWrittenElements);fflush(stdout);
// if(buff.goToStartWriteIndex == true){
//   printf("step1 \n");fflush(stdout);
//   printf("(countWrittenElements - BUFFER_SIZE_DATA_FROM_SPI)=%d \n",(countWrittenElements - BUFFER_SIZE_DATA_FROM_SPI));fflush(stdout);
//   //printf("(BUFFER_SIZE_DATA_FROM_SPI - countWrittenElements)=%u \n",(BUFFER_SIZE_DATA_FROM_SPI - countWrittenElements));fflush(stdout);
//   int i=0; printf("i=%d \n",i);fflush(stdout);
//       for(i=buff.readIndex; i<BUFFER_SIZE_DATA_FROM_SPI; ++i){
//           DataToEth[i] = get(&buff, DataFromSPI);
//       }
//       printf("i=%d \n",i);fflush(stdout);
//       i+=1;printf("i=%d \n",i);fflush(stdout);
//       for(int k=0; k<(countWrittenElements - BUFFER_SIZE_DATA_FROM_SPI); ++k, ++i){
//           DataToEth[i] = get(&buff, DataFromSPI);
//       }
      
//       buff.goToStartWriteIndex=false;

      // for(i; i<buff.writeIndex; ++i){
      //     DataToEth[i] = get(&buff, DataFromSPI);
      // }
        //  for (int k = 0; k < countWrittenElements; k++) {
        //      printf("DataToEth[%d] = %d\n", k, DataToEth[k]);
        //  }
// }else{
//       for(int i=buff.readIndex; i<countWrittenElements; ++i){
//           DataToEth[i] = get(&buff, DataFromSPI);
//       }
//}

//printf("check=%d \n",(BUFFER_SIZE_DATA_FROM_SPI - countWrittenElements+buff.writeIndex));fflush(stdout);
      // for(int i=buff.readIndex; i<countDataToEth; ++i){
      //     DataToEth[i] = get(&buff, DataFromSPI);
      // }
      //printf("buff->readIndex2=%d \n",buff.readIndex);fflush(stdout);
      //printf("buff.writeIndex-buff.readIndexl=%d \n",buff.writeIndex-buff.readIndex);fflush(stdout);
      //for(int i=0; i<100; ++i){printf(" DataToEth[%d] = %d\n", i, DataToEth[i]);fflush(stdout);}

      //printf("result send=%d \n", result);fflush(stdout);
        //clt_sock.send(&DataToEth[0], countWrittenElements);
        //printf("result send=%d \n", result);fflush(stdout);
        // for (int i = 0; i < sizeMainBuffer; i++) {
        //     printf("MainBuffer[%d] = %x\n", i, MainBuffer[i]);
        // }
        //printf("MainBuffer was send, VectorBytes.size()=%u \n", VectorBytes.size());fflush(stdout);
        //VectorBytes.clear();
    //}
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
        //printf("count = %u\n", count);
    chipSelect = 0; // Select device
    spi.write(txBufferMsv, 3, rxBufferMsv, 3);
    chipSelect = 1; // Deselect device
                                    //for test master get reference samples from slave
                                        // for(uint8_t i=0; i<3; ++i){
                                        //     printf("rxBufferMsv[%d] = %d \n", i, rxBufferMsv[i]);
                                        // }
    if(flagGetDataSPI==false){
      /*Сигнал о прекращении опроса по SPI может поступить
       как в цикле for (ниже), так и до наступления момента входа в этот цикл.
       Без этого куска кода в массив прибавится Байт*/
        countWrittenElements=count;
        count=0;  //printf("STEP 2 \n");fflush(stdout);
        eventFlags.clear(GET_DATA_FROM_SPI_FLAG);
        //eventFlags.set(MAINBUFFER_READY_FROM_SPI_FLAG);  //printf("STEP 3 \n");fflush(stdout);
        flagEnableGetSPI =false; /*опрашивать SPI или нет*/
    }

    if(flagEnableGetSPI){
        for(uint8_t i=0; i<3; i++){ //24bit ADC
            //VectorBytes.push_back(rxBufferMsv[i]);
            put(&buff, MainBuffer, rxBufferMsv[i]);
            if(i==1){eventFlags.set(READY_TO_SEND_DATA_FLAG);} /*чтобы READ из буфера в порт не стартовал раньше WRITE в буфер данных*/
            
            count++;        /*пояснение к абзацу выше: здесь может в массив прибавится тот самый Байт*/
               if(flagGetDataSPI==false){
                countWrittenElements=count;
                count=0;  //printf("STEP 2 \n");fflush(stdout);
                eventFlags.clear(GET_DATA_FROM_SPI_FLAG);
                //eventFlags.set(MAINBUFFER_READY_FROM_SPI_FLAG);  //printf("STEP 3 \n");fflush(stdout);
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
        eventFlags.set(PUSH_DATA_TO_ETH_FLAG);
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

            eventFlags.clear(PUSH_DATA_TO_ETH_FLAG);
            eventFlags.clear(READY_TO_SEND_DATA_FLAG);
            eventFlags.set(STOP_PUSH_DATA_TO_ETH_FLAG);
            flagGetDataSPI=false;
        
        printf("buff->readIndex=%d \n",buff.readIndex);fflush(stdout);
        printf("buff->writeIndex=%d \n",buff.writeIndex);fflush(stdout);
        printf("countWrittenElements=%d \n",countWrittenElements);fflush(stdout);        
        //printf("wasSend=%d \n",wasSend);fflush(stdout);
        printf("wasSend3=%d \n",wasSend3);fflush(stdout);
        printf("bool test1=%d \n",test1);fflush(stdout);
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
  int r;
  int res_put;

  init(&buff);


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