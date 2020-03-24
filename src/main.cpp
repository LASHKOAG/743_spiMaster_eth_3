/*
743 spi master eth 3
struct1
добавим файл структур
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

#include "rtos/Thread.h"
//#include "rtos/rtos_idle.h"
#include "platform/mbed_critical.h"

#include "platform/CircularBuffer.h"

#define BUF_SIZE    400

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

TCPSocket srv;  //TCPServer was migrate to TCPSocket
TCPSocket *clt_sock;
SocketAddress clt_addr;
EthernetInterface eth;
EventFlags eventFlags;
// CriticalSectionLock csLock;

CircBuff *CircBuffer= new CircBuff();
//CircBuffer->initStructCircularBuffer();
// struct structBuffer{
//     int8_t MainBuffer[sizeMainBuffer];
// }structBuff;

    map <char, char> myMap;
    vector <char> myVec;
    vector<char> myVec2;

template < typename T>
std::pair<bool, char > findInVector(const std::vector<T>  & vecOfElements, const T  & element)
{
    std::pair<bool, char > result;

    // Find given element in vector
    auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);

    if (it != vecOfElements.end())
    {
        result.second = distance(vecOfElements.begin(), it);
        result.first = true;
    }
    else
    {
        result.first = false;
        result.second = -1;
    }

    return result;
}

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
                    wasSend1+=clt_sock->send(&TempBuf[0], counter);
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

                    wasSend2+=clt_sock->send(&TempBuf[0], counter);
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

int ethernetInterfaceInit()
{
    //EthernetInterface eth;
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
        
        srv.open(&eth);                         /* Open the server on ethernet stack */
        int rrr = srv.bind(eth.get_ip_address(), 80);     /* Bind the HTTP port (TCP 80) to the server */
        printf("rrr: %d\r\n",rrr);
        int rett = srv.listen(5);                          /* Can handle 5 simultaneous connections */
        printf("rett: %d\r\n",rett);

        return 0;
    }
    return -2;
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

void deleteTask(){
    char numberTask = (char)120;
    if(findInVector(myVec, numberTask).first == true){
        int index = findInVector(myVec, numberTask).second;
        cout << "index = findInVector= " << index << endl;
        myVec.erase(myVec.begin() + index -1);
        myMap.erase(numberTask);

        // std::map<char,int>::iterator iter;
        // iter=myMap.find(numberTask);
        // myMap.erase(iter);
    }else{
        printf("\n-------kosyak------\n");
    }

}

void getCommandFromPort3(std::string& CommandFromPort)
{
    int32_t numberCase=0;
    std::cout << " Command from port <- " << CommandFromPort << std::endl;

    string strGetDataSPI="2";
    string strStopGetDataSPI="4";
    string strDate="date";
    string deleteTasks="9";

    if (CommandFromPort.compare(0,strGetDataSPI.size(), strGetDataSPI) == 0){numberCase=2; printf("case 2\n");}
    if (CommandFromPort.compare(0,strStopGetDataSPI.size(), strStopGetDataSPI) == 0){numberCase=4; printf("case 4\n");}
    if (CommandFromPort.compare(0,strDate.size(), strDate) == 0){numberCase=6; printf("case 6\n");}
    if(std::stoi(CommandFromPort) == CMD_HAL_STOP_TS_TASK){numberCase = CMD_HAL_STOP_TS_TASK;}
    if (CommandFromPort.compare(0,deleteTasks.size(), deleteTasks) == 0){numberCase=9; printf("case 9\n");}

    /* обнулить входящий массив после того как он отработает*/
    
    tcp_packet_t packet;

    switch (numberCase) {
        case 2:
            printf("\nget command from port: %s\n", &CommandFromPort);
            fflush(stdout);
            eventFlags.set(GET_DATA_FROM_SPI_FLAG);
            eventFlags.set(PUSH_DATA_TO_ETH_FLAG);
            flagGetDataSPI=true;
            flagEndOfMeasurementSPI=true;
            break;
        case 3:
            printf("\nget command from port: %s \n", &CommandFromPort);
            fflush(stdout);
            printf("\n onOffLed() function has to work!\n");
            fflush(stdout);
            onOffLed();
            break;
        case 4:
            printf("\nget command %s from port\n", &CommandFromPort);
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
            printf("\nget command %s from port\n", &CommandFromPort);
            fflush(stdout);
            printf("\n port close()\n");
            fflush(stdout);


            // srv.close();
            // //eth.
            // eth.disconnect();
            // flag1=false;
            // wait(120);
            break;
        case 6:
                std::cout << "step case 6" << std::endl;
            break;
        case 9:
            char numberTask = (char)120;
            if(findInVector(myVec2, numberTask).first == true){
                // int index = findInVector(myVec, numberTask).second;
                // cout << "index = findInVector= " << index << endl;
                // myVec.erase(myVec.begin() + index -1);
                // //auto itMap = myMap.begin();///создаем итератор на начало myМap
                // auto itMap = myMap.find(numberTask);
                // myMap.erase(itMap);

                auto itmyVec2 = find(myVec2.begin(), myVec2.end(), numberTask);
                myVec2.erase(itmyVec2);
                    cout << "myVec2.size() " << myVec2.size() << endl;
                //myVec2.erase(itmyVec2);
                  //  cout << "myVec2.size() " << myVec2.size() << endl;
                myVec2.shrink_to_fit();

                // auto index = findInVector(myVec2, numberTask).second;
                // cout << "index = findInVector2= " << index << endl;
                // cout << "myVec2.size() " << myVec2.size() << endl;
                // //myVec2.erase(myVec2.begin() + index, myVec2.begin() + index+1);
                // myVec2.erase(myVec2.begin() + index);
                // cout << "myVec2.size() " << myVec2.size() << endl;
                // myVec2.shrink_to_fit();
                //                 myVec2.erase(myVec2.begin() + index);
                // cout << "myVec2.size() " << myVec2.size() << endl;
                // myVec2.shrink_to_fit();
                //myVec2.resize(myVec2.size());


                // std::map<char,int>::iterator iter;
                // iter=myMap.find(numberTask);
                // myMap.erase(iter);
            }else{
                printf("\n-------kosyak------\n");
            }
        break;
        /*
        case CMD_HAL_STOP_TS_TASK:
        	    printf("Command: stop TIMESIGNAL task\n");
            int res = TCP_EC_SUCCESS;
                // if (ts_task != NULL){
                // ts_task->stop();
                // }
            //заслать ответ
            tcp_packet_t ans;
            memset(&ans, 0x00, sizeof(tcp_packet_t));
            ans.command = CMD_ANSWER;
            ans.length = sizeof(ans.command)+sizeof(int);
            ans.buff = new char[ans.length];
            memcpy(&ans.buff[0], (char*)&packet.command, sizeof(packet.command));
            memcpy(&ans.buff[sizeof(packet.command)], (char*)&res, sizeof(int));
            //int cnt = send_packet(&ans);
            delete[] ans.buff;
            break;
*/
    }
}

void process_buffer(char* buf, int len, TCPSocket * ptrSock)
{
    ptrSock->send("Hello",sizeof("Hello"));
    if (!buf) return;
        //как минимум 2 по 4 байта (команда и длина)
    if (len < 8) return;
    tcp_packet_t packet;
    memset(&packet, 0x00, sizeof(tcp_packet_t));
    memcpy((char*)&packet.command, &buf[0], sizeof(packet.command));
    memcpy((char*)&packet.length, &buf[sizeof(packet.command)], sizeof(packet.length));
    if ((len-8) > 0){
	packet.buff = new char[len-8];
        memcpy(&packet.buff[0], &buf[sizeof(packet.command)+sizeof(packet.length)], len-8);
    }
    switch (packet.command){
        case CMD_HAL_STOP_TS_TASK:{
            	    printf("Command: stop TIMESIGNAL task\n");
            int res = TCP_EC_SUCCESS;
            // if (ts_task != NULL){
            // ts_task->stop();
            // }
            //заслать ответ
            tcp_packet_t ans;
            memset(&ans, 0x00, sizeof(tcp_packet_t));
            ans.command = CMD_ANSWER;
            ans.length = sizeof(ans.command)+sizeof(int);
            ans.buff = new char[ans.length];
            memcpy(&ans.buff[0], (char*)&packet.command, sizeof(packet.command));
            memcpy(&ans.buff[sizeof(packet.command)], (char*)&res, sizeof(int));
            //int cnt = send_packet(&ans);
            int cnt = ptrSock->send(&ans,sizeof(ans));
            //int cnt = ptrSock->send(&ans,sizeof(ans));
            delete[] ans.buff;
            break;
        }
    }
    // sSend2+=clt_sock->send(&TempBuf[0], counter);
}

void call_resultTask(TCPSocket * ptrSock2){
    while(1){
    //ptrSock2->send(&myVec,myVec.size());
    //ptrSock2->send(&myMap,myMap.size());
    //csLock.enable ();
    //CriticalSectionLock lock;
    // for (auto it = myMap.begin(); it != myMap.end(); ++it){
    //     // cout << it->first << " : " << it->second << endl;
    //     ptrSock2->send(&it->first, 1);
    //     ptrSock2->send(&it->second, 1);
    //     cout << it->first << endl;
    //     cout << it->second << endl;
        
    // }
    myVec2.shrink_to_fit();
    cout << "c_rT myVec2.size()=" << myVec2.size()<< endl;
   
    myVec2.shrink_to_fit();

    if(myVec2.size() !=0 ){
        int sendBytes=ptrSock2->send(&myVec2, myVec2.size());
         cout << "c_rT sendBytes=" << sendBytes << endl;
            for (auto it = myVec2.begin(); it != myVec2.end(); ++it){
            // cout << it->first << " : " << it->second << endl;
            //ptrSock2->send(&it, 1);
            cout << *it<< endl;
            
        }
    }
    //csLock.disable();
    //CriticalSectionLock unlock;
    cout << " =========== " << endl;
    wait(5);
    }

}


int main()
{
    printf("\n======== 1-start ======================\n");  fflush(stdout);
    printf("Basic HTTP server example\n");

    spi.format(8,3);        // Setup:  bit data, high steady state clock, 2nd edge capture
    spi.frequency(1000000); //1MHz
    int r;
    int res_put;

    //CircBuffer->initStructCircularBuffer();


    drdyIN.rise(&drdyINHandlerRise);   //interrupt DRDY flag from slave (hardware)
    spiThread.start(call_spiThread2);  //get data SPI from Slave equipment
    sockSendThread.start(call_sockSendThread);  //send data in ethernet port to client
    

    int resEth = ethernetInterfaceInit();

    char Recv_msv[100];                  /* buffer for command from port */
    string strRecv_msv;                  /* buffer for command from port */
    //while(1){

    if(resEth==0){
        //srv.accept(&clt_sock, &clt_addr);
        clt_sock = srv.accept();  //return pointer of a client socket
        clt_sock->getpeername(&clt_addr);  //this will fill address of client to the SocketAddress object
            printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
         flagMainThread=true;
         process_buffer(Recv_msv, 100, clt_sock);
    }
resultTaskThread.start(call_resultTask, clt_sock);
    while(flagMainThread) {

        // srv.accept(&clt_sock, &clt_addr);
        //           printf("\naccept %s:%d\n", clt_addr.get_ip_address(), clt_addr.get_port());
        clt_sock->recv(&Recv_msv, 100);
            printf("Recv recv_msv %s \n", Recv_msv);
            printf("strlen Recv_msv =%d\n", strlen(Recv_msv));
        strRecv_msv=Recv_msv;

        getCommandFromPort3(strRecv_msv);



    myMap.emplace(stoi(strRecv_msv),0);
    myVec.emplace_back(stoi(strRecv_msv));
    
    myVec2.emplace_back(stoi(strRecv_msv));
    myVec2.emplace_back(0);

    if (!buf.full()) {
        buf.push(stoi(strRecv_msv));
        buf.push(0);

        bytes_written +=2;
    }

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
    }
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