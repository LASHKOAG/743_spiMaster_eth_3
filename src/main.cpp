/*
743 spi master eth 3
queue1
*/

#include "mbed.h"
#include "platform/mbed_thread.h"
#include "EthernetInterface.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include <time.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "hwclockstm.h"
#include "circbuff.h"
#include <string>
#include <chrono>
#include <math.h>
//#include "chrono_io"
#include <queue>
#include "socket_struct.h"
//#include <map>    //подключили библиотеку для работы с map
#include <algorithm>
//#include <unistd.h>
#include "createport.h"

#include "rtos/Thread.h"
//#include "rtos/rtos_idle.h"
#include "platform/mbed_critical.h"

#include "platform/CircularBuffer.h"



using namespace std::chrono;




//#define BUF_SIZE    400
#define MEMP_NUM_NETCONN 8

//CircularBuffer<int, BUF_SIZE> buf;
//uint32_t bytes_written = 0;

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
bool flagReadyTSPortThread = false;
bool flagAcceptPort500 = false;
bool flagAllowRecvFromPort=true;
bool flag_cmd_check_answer = false;
bool flag_busy_port=false;
bool flag_get_log=false; // init false;

char Recv_msv150[100];
char Recv_msvTest[100];
char Recv_msvTest2[100];
char Recv_msvTest3[100];

SPI spi(PD_7, PA_6, PA_5, PA_4); // mosi, miso, sclk, ssel
DigitalOut chipSelect(PA_4);
InterruptIn drdyIN(PE_3);

Thread spiThread;
Thread timeSignalPortThread;
Thread resultTaskThread;
Thread generalPortThread;
Thread getCommandPortThread;
//Thread portThread150;

// TCPSocket srv, srv150;  //TCPServer was migrate to TCPSocket
// TCPSocket *clt_sock, *clt_sock150;
// SocketAddress clt_addr, clt_addr150;
EthernetInterface eth;
EventFlags eventFlags;

//Thread signalPortThread;
// CriticalSectionLock csLock;


//map <char, char> myMap;
vector <char> myVec;
vector <string> myVec2;
    std::vector <int32_t> CMD_ARRAY; //вектор набора существующих задач
    std::vector<int32_t>::iterator iter;


queue <string> QueueTasks;


CircBuff *CircBuffer= new CircBuff();
//-----------------------------------------------------------------------------------------------------------------------
#define CMD_CHECK_CONNECTION        0   //проверка связи с устройством
//----------------------------------------------------------------------------------------------------------------------

//======================================================================================
//отправка команды от сервера к клиенту о проверке связи с устройством
typedef struct {
    uint8_t IDstm;          //№ stm 
    uint8_t in_command;     //№ команды из перечня команд запросов
    int8_t out_command;     //№ команды из перечня команд ответов
}tcp_server_answer;
//======================================================================================
typedef struct{
    uint32_t id_task;
    uint8_t number_command;
}tcp_sTask;

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

void call_timeSignalPortThread(CreatePort* port)
{
    bool a=true;
    bool repeatConnect=false;
    bool needRepeatConnect=false;

    //CreatePort *port500 = new CreatePort(eth, 500);
    // if(port->flag_AcceptPort==0){ //связь с клиентом установлена
    //     flagReadyTSPortThread=true;
    // }else{                           //связь с клиентом не установлена
    //         printf("Problem with PORT\n");
    //     port->clt_sock->close();
    //     if(port->repeatConnect()==0){  //пытаемся переподключиться
    //         flagReadyTSPortThread=true;  //переподключились
    //     }
    // }

    int sizeTempBuf=100;
    char TempBuf[sizeTempBuf];
    int counter=0;
    bool flagSendFirstPartToEth=true;
    flagReadyTSPortThread=true;
    int step=0;
    // while(a){
    //             //int check = port500->clt_sock->send("CHECK", strlen("CHECK"));
    //     int check = port500->clt_sock->send("0xFF", strlen("0xFF"));
    //     printf("check = %d\n", check);
    //     if(check<0){               //порт назначения закрыт клиентом
    //         printf("Problem with PORT\n");
    //         port500->clt_sock->close();
    //         //a=false;
    //         if(port500->repeatConnect()==0){  //пытаемся переподключиться
    //             flagReadyTSPortThread=true;  //переподключились
    //         }
    //     }
    // }

    while(flagReadyTSPortThread) {
        needRepeatConnect=false;
        if(eventFlags.wait_all(PUSH_DATA_TO_ETH_FLAG | READY_TO_SEND_DATA_FLAG, osWaitForever, false)) {
            step=1;
                        flagSendFirstPartToEth=true;
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
                    //wasSend1+=clt_sock.send(&TempBuf[0], counter);
                    int resultSendPort1 = port->clt_sock->send(&TempBuf[0], counter);
                    //wasSend1+=port500->clt_sock->send(&TempBuf[0], counter);
                    wasSend1+=resultSendPort1;
                    counter=0;
                     if(resultSendPort1<0){               //порт назначения закрыт клиентом
                             printf("Problem with PORT\n");
                         port->clt_sock->close();
                         needRepeatConnect=true;
                         flagAllowRecvFromPort=false;
                         eventFlags.clear(PUSH_DATA_TO_ETH_FLAG );
                         eventFlags.clear(READY_TO_SEND_DATA_FLAG);
                         eventFlags.clear(GET_DATA_FROM_SPI_FLAG);
                        flagGetDataSPI=false;
                        flagEndOfMeasurementSPI=false;
                        flagReadyTSPortThread=false;
                        CircBuffer->clearCircularBuffer();  /*переставляем индексы кольцевого буффера в начало*/
                        counter=0;
                        wasSend1=0;/*для отладки*/
                        wasSend2=0;/*для отладки*/
                        step=0;
                        numberBytesSendToEth=0;
                        numberBytesTakenFromSPI=0;
                        flagSendFirstPartToEth=false;
                    //     repeatConnect=true;
                    //     flagSendFirstPartToEth=false;                   //выйти и обнулить всё(не доделал проверить обнуление всего)
                    //     flagReadyTSPortThread=false;
                     }
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

                    //wasSend2+=clt_sock.send(&TempBuf[0], counter);
                    int resultSendPort = port->clt_sock->send(&TempBuf[0], counter);
                    wasSend2+=resultSendPort;
                    counter=0;
                    if(resultSendPort<0){               //порт назначения закрыт клиентом
                             printf("Problem with PORT\n");
                        port->clt_sock->close();
                        needRepeatConnect=true;
                        flagAllowRecvFromPort=false;
                        eventFlags.clear(PUSH_DATA_TO_ETH_FLAG );
                        eventFlags.clear(READY_TO_SEND_DATA_FLAG);
                        eventFlags.clear(GET_DATA_FROM_SPI_FLAG);
                        flagGetDataSPI=false;
                        flagEndOfMeasurementSPI=false;
                        flagReadyTSPortThread=false;
                        CircBuffer->clearCircularBuffer();  /*переставляем индексы кольцевого буффера в начало*/
                        counter=0;
                        wasSend1=0;/*для отладки*/
                        wasSend2=0;/*для отладки*/
                        step=0;
                        numberBytesSendToEth=0;
                        numberBytesTakenFromSPI=0;
                        flagSendFirstPartToEth=false;
                        difference=0;
                    //     repeatConnect=true;
                    //     flagSendFirstPartToEth=false;                   //выйти и обнулить всё(не доделал проверить обнуление всего)
                    //     flagReadyTSPortThread=false;
                    }

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
    if(needRepeatConnect){port->repeatConnect();printf("new connection get\n"); flagReadyTSPortThread=true; flagAllowRecvFromPort=true;}
        
    }
    //delete port;
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
        printf("\n======== step EthernetInterfaceFunction() ======================\n");  fflush(stdout);
        
    int resultEthSetNetwork = eth.set_network("192.168.4.177","255.255.255.0","192.168.4.1");   /* set network settings */
        printf("Set Net: %d\r\n",resultEthSetNetwork);  fflush(stdout);
    if(resultEthSetNetwork !=0){printf("Error NO set_network \n"); return -1;}

    //int resEthConnect = -1;
    //while(resEthConnect != 0){printf("into while \n");  resEthConnect = eth.connect(); printf("resEthConnect: %d\r\n",resEthConnect);}
       // printf("after while \n"); return -1;
    
    if(eth.connect() !=0) {
            printf("Error connecting \n");
        return -1;
    }else{
            printf("\nstep eth.connect()\n");  fflush(stdout);
        const char *ip = eth.get_ip_address();  // Show the network address
            printf("The Server IP address is: %s\n", ip ? ip : "No IP");  fflush(stdout);
            //printf("The Server IP address is '%s'\n", eth.get_ip_address());  fflush(stdout);
        return 0;
    }
    
}

void f_cmd_check_connection(CreatePort* port){
    char test1[10];
    int8_t test2[10];
    string test3;
    //int32_t test4[10];

    tcp_server_answer sServer_Answer;
    sServer_Answer.IDstm=100;
    sServer_Answer.in_command=0;
    sServer_Answer.out_command=1;
    port->clt_sock->send((char*)&sServer_Answer, sizeof(sServer_Answer));
    
    //port->clt_sock->send(test1, sizeof(test1));
    //port->clt_sock->send(test2, sizeof(test2));
    //port->clt_sock->send(test4, sizeof(test4));
    //port->clt_sock->send(test3, sizeof(test3));
    //port->clt_sock->send(sServer_Answer.IDstm, sizeof(sServer_Answer));
}

//void getCommandFromPort3(CreatePort* port, std::string& CommandFromPort)
void getCommandFromPort3(std::string& CommandFromPort)
    //void getCommandFromPort3(queue& QueueTasksFromPort)
{
            cout << "myQueue.size() " << QueueTasks.size() << endl;
            cout << "myQueue.front() " << QueueTasks.front() << endl;
            //QueueTasks.
    // while(!QueueTasks.empty()) {
    //     cout << QueueTasks.front() << " ";
    //     //QueueTasks.pop();
    // }
    int32_t numberCase=0;
    std::cout << " Command from port <- " << CommandFromPort << std::endl;

    string strCHECK_CONNECTION = "0";
    string strGetDataSPI="2";
    string strStopGetDataSPI="4";
    string strDate="date";
    string strSeven="7";

    if (CommandFromPort.compare(0,strCHECK_CONNECTION.size(), strCHECK_CONNECTION) == 0){numberCase=0; printf("case 0\n");}

    if (CommandFromPort.compare(0,strGetDataSPI.size(), strGetDataSPI) == 0){numberCase=2; printf("case 2\n");}
    if (CommandFromPort.compare(0,strStopGetDataSPI.size(), strStopGetDataSPI) == 0){numberCase=4; printf("case 4\n");}
    if (CommandFromPort.compare(0,strDate.size(), strDate) == 0){numberCase=6; printf("case 6\n");}
    if (QueueTasks.front().compare(0,strSeven.size(), strSeven) == 0){numberCase=7; printf("case 7\n");}

QueueTasks.pop();
    /* обнулить входящий массив после того как он отработает*/

    switch (numberCase) {
        case 0:
            printf("case 000\n");
            flag_cmd_check_answer = true;
            //Thread testThread;
            //testThread.start();
            //f_cmd_check_connection(port);
            break;
        case 2:
            //printf("\nget command from port: %s\n", &CommandFromPort);
            //fflush(stdout);
            if(flagReadyTSPortThread){
                eventFlags.set(GET_DATA_FROM_SPI_FLAG);
                eventFlags.set(PUSH_DATA_TO_ETH_FLAG);
                flagGetDataSPI=true;
                flagEndOfMeasurementSPI=true;
            }else{printf("\n port for time signal not found \n");}

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
        case 7:
            printf("case 77\n");

            break;
    }
}
/*
void call_generalPortThread(){
    CreatePort *port80 = new CreatePort(eth, 80);
    char *ReceivedMsv = new char[100]{0};               // buffer for command from port 
    string strRecv_msv;
    while(1){
        port80->clt_sock->recv(ReceivedMsv, 100);
            printf("ReceivedMsv port80 = %s \n", ReceivedMsv);
            printf("strlen ReceivedMsv80 = %d\n", strlen(ReceivedMsv));
        strRecv_msv=ReceivedMsv;
        getCommandFromPort3(CreatePort* port,strRecv_msv);
    }
    delete[] ReceivedMsv;
}
*/

int send_log_message(CreatePort* port, char* msg, int len)
{
    if (msg == NULL) return 0;
    if (len <= 0) return 0;
    //if (!sock) return -1;
    
    tcp_packet_t ans;       //заслать ответ
    memset(&ans, 0x00, sizeof(tcp_packet_t));
    ans.command = CALLBACK_LOG_MESSAGE;
    //ans.length = len;
    ans.length = strlen(msg);
    //ans.buff = new char[len];
    ans.buff = new char[strlen(msg)];
    //memcpy(&ans.buff[0], &msg[0], len);
    memcpy(&ans.buff[0], &msg[0], strlen(msg));
   
    //int32_t len_send = sizeof(ans.command)+sizeof(ans.length) + len;
    int32_t len_send = sizeof(ans.command)+sizeof(ans.length) + strlen(msg);
    //int32_t byteSend = port->clt_sock->send(&ans, len_send);
    char *TempMSV = new char[len_send];
    int32_t pos1=0;
    memcpy(&TempMSV[pos1], (char*)&ans.command, sizeof(ans.command));
        pos1+=sizeof(ans.command);
    memcpy(&TempMSV[pos1], (char*)&ans.length, sizeof(ans.length));
        pos1+=sizeof(ans.length);
    //memcpy(&TempMSV[pos1], (char*)&ans.buff[0], len);
    memcpy(&TempMSV[pos1], (char*)&ans.buff[0], strlen(msg));

    int32_t byteSend = port->clt_sock->send(&TempMSV[0], len_send);
        // printf("ans.command = %u\t", ans.command);
        // printf("ans.length = %u\t", ans.length);
        // printf("ans.buff = %s\n", ans.buff);
        printf("log msg = %s\n", msg);
    delete[] TempMSV;
    delete[] ans.buff;
    return byteSend;
}

void dispatcher(CreatePort* port, char* buf, int len){

    // int32_t task;
    // task = buf[0];
    // printf("task # = %d\n", task);

    // if(QueueTasks.front() !="9"){
    //     QueueTasks.pop(); printf("QueueTasks.pop()\n");
    // }else{
    //     task = stoi(QueueTasks.front());
    //     printf("task # = %d\n", task);
    
    //task = stoi(QueueTasks.front());
    // printf("task # = %d\n", task);
    
    if (!buf) return;
        printf("len = %d\n", len);    
    if (len < 8) return;  //как минимум 2 по 4 байта (команда и длина)

    tcp_packet_t packet;
    memset(&packet, 0x00, sizeof(tcp_packet_t));
    memcpy((char*)&packet.command, &buf[0], sizeof(packet.command));
    memcpy((char*)&packet.length, &buf[sizeof(packet.command)], sizeof(packet.length));
    if ((len-8) > 0){
        packet.buff = new char[len-8];
        memcpy(&packet.buff[0], &buf[sizeof(packet.command)+sizeof(packet.length)], len-8);
    }

        printf("packet.command = %u \n", packet.command);
    //=====проверка, реализована ли запрашиваемая функциональность==========================================
    iter = std::find(CMD_ARRAY.begin(), CMD_ARRAY.end(), packet.command); // поиск поступившего номера задач среди реализованных
        if (iter != CMD_ARRAY.end()){
                std::cout << "Element Found" << std::endl;  //функционал реализован, выполняем
                char* Msg= new char[100];
                sprintf(Msg, "Task %u was found", packet.command);
            if(flag_get_log){send_log_message(port,Msg, strlen("Msg"));}
            delete[] Msg;
            switch (packet.command)  //switch (task)
            {
            case 0:    
                // tcp_server_answer sServer_Answer;

                // sServer_Answer.IDstm=100;
                // sServer_Answer.in_command=0;
                // sServer_Answer.out_command=1;
                // while(flag_busy_port == true){}  //ждем когда можно отправлять, порт занят другой задачей
                // if(flag_busy_port==false){
                //     flag_busy_port=true;
                //     port->clt_sock->send((char*)&sServer_Answer, sizeof(sServer_Answer));
                //     flag_busy_port=false;
                // }
                break;
            case CMD_SET_TIME:{   //2
                //!!!необходимо вкатать в прибор время
                if (!packet.buff) break;
                    // time_t seconds = time(NULL);
                    // printf("Time as seconds since January 1, 1970 = %u\n", (unsigned int)seconds);
                    // printf("Time as a basic string = %s", ctime(&seconds));
                time_t* t;
                t = (time_t*)&packet.buff[0];
                    printf("Command: Set time %d\n", *t);
                
                if (t != 0){
                    time_t t1 = time(NULL);  printf("Time on board was = %u\n", (unsigned int)t1);  printf("Time as a basic string = %s", ctime(&t1));  fflush(stdout);
                    //if (abs(t1-(*t)) > 2){  //не компилируется
                    //if ((t1-(*t)) > 2 || (t1-(*t)) < 2){ //проверить!
                    //time_t x = (t1-(*t));
                    if (t1 > *t + 2 || *t >= t1 +2 ){ //проверить!
                        //printf("setting time\n");
                        set_time(*t);
                        if(flag_get_log){send_log_message(port,"The time was corrected", strlen("The time was corrected"));}
                    }
                }
                //заслать ответ
                tcp_packet_t ans;
                memset(&ans, 0x00, sizeof(tcp_packet_t));
                ans.command = CMD_ANSWER;
                ans.length = sizeof(ans.command);
                ans.buff = (char*)&packet.command;
                int32_t len_send = sizeof(ans.command) + sizeof(ans.length) + sizeof(packet.command);
                int8_t *TempMSV= new int8_t[len_send];
                int32_t pos1=0;
                memcpy(&TempMSV[pos1], (char*)&ans.command, sizeof(ans.command));
                    pos1+=sizeof(ans.command);
                memcpy(&TempMSV[pos1], (char*)&ans.length, sizeof(ans.length));
                    pos1+=sizeof(ans.length);
                memcpy(&TempMSV[pos1], (char*)&ans.buff[0], sizeof(packet.command));
                    pos1+=sizeof(packet.command);

                port->clt_sock->send(&TempMSV[0], len_send);

                    printf("send SET_TIME\n");
                delete[] TempMSV;
                break;
            }
            case CMD_HAL_INIT:{//complete //5
                    printf("Command: Init HAL\n");
                if(flag_get_log){send_log_message(port,"Command: Init HAL", strlen("Command: Init HAL"));}
                flag_get_log=false;
                // if (ts_task != NULL){
                // printf("TIMESIGNAL ALREADY RUNNING!\n");
                // int res = TCP_EC_TASK_IN_PROCESS;
                // tcp_packet_t ans;
                // memset(&ans, 0x00, sizeof(tcp_packet_t));
                // ans.command = CMD_ANSWER;
                // ans.length = sizeof(ans.command)+sizeof(res);
                // ans.buff = new char[ans.length];
                // memcpy(&ans.buff[0], (char*)&packet.command, sizeof(packet.command));
                // memcpy(&ans.buff[sizeof(packet.command)], (char*)&res, sizeof(res));
                // int cnt = send_packet(&ans);
                // delete[] ans.buff;
                // break;
                // }
             /*   int res;
                HD = NULL;
                if (CHECK_EC(res = init_hal())){
                printf("init hal success\n");
                HD = get_descriptor();
                if (HD == NULL)
                    printf("error getting descriptor\n");
                }
                else{
                printf("init hal error: %d\n", res);
                }
                if ((HD == NULL) && (CHECK_EC(res)))
                res = TCP_EC_ERROR_DESCRIPTOR;
                //заслать ответ
                tcp_packet_t ans;
                memset(&ans, 0x00, sizeof(tcp_packet_t));
                ans.command = CMD_ANSWER;
                ans.length = sizeof(ans.command)+sizeof(res);
                ans.buff = new char[sizeof(packet.command)+sizeof(res)];
                memcpy(&ans.buff[0], (char*)&packet.command, sizeof(packet.command));
                memcpy(&ans.buff[sizeof(packet.command)], (char*)&res, sizeof(res));
                int cnt = send_packet(&ans);
                delete[] ans.buff;
                break;*/
            }
            case CMD_HAL_GET_DESC:{  //9
                    printf("Command: get descriptor\n");
                if(flag_get_log){send_log_message(port,"Command: get descriptor", strlen("Command: get descriptor"));}
                tcp_packet_t ans;
                memset(&ans, 0x00, sizeof(tcp_packet_t));
                ans.command = CMD_ANSWER;
                //----------------------------------------------------------------------------------
                char dev_name1[]="stm-01";
                tcp_hal_desc_t desc;
                desc.id=10;
                desc.dev_lib_version = 20;
                desc.interface_version = 30;
                desc.server_version = 40;
                desc.len_dev_name = strlen(dev_name1);
                desc.dev_name = dev_name1;
                //---------------------------------------------------------------------------------
                //if (!HD){
                if (false){
                        printf("descriptor is NULL\n");
                    ans.length = sizeof(ans.command);
                    ans.buff = new char[sizeof(packet.command)];
                    memcpy(&ans.buff[0], (char*)&packet.command, sizeof(packet.command));
                }
                else{
                        printf("descriptor is not null\n");
                    //int len = strlen(HD->dev_name);
                    int32_t len_dev_name = strlen(desc.dev_name);
                    ans.length = sizeof(ans.command)+4*sizeof(unsigned short)+sizeof(unsigned int)+len_dev_name;
                        //printf("ans.length =%d \n", ans.length);
                    ans.buff = new char[ans.length];
                    int32_t pos = 0;
                    if(ans.buff == NULL){                   // проверить выделилась ли память?
                            printf("ans.buff == NULL\n");
                        int8_t *TempMSV= new int8_t[12];
                        ans.length = 2*sizeof(packet.command); 
                        memcpy(&ans.buff[pos], (char*)&packet.command, sizeof(packet.command));
                            pos+= sizeof(packet.command);
                        memcpy(&ans.buff[pos], (char*)TCP_EC_UNSUCCESS, sizeof(packet.command));
                            pos=0;
                        delete[] TempMSV;
                    }
                    memcpy(&ans.buff[pos], (char*)&packet.command, sizeof(packet.command));
                        pos+= sizeof(packet.command);
                    //memcpy(&ans.buff[pos], (char*)&HD->id, sizeof(unsigned short));
                    memcpy(&ans.buff[pos], (char*)&desc.id, sizeof(unsigned short)); //1
                        pos+= sizeof(unsigned short);
                    //memcpy(&ans.buff[pos], (char*)&HD->dev_lib_version, sizeof(unsigned short));
                    memcpy(&ans.buff[pos], (char*)&desc.dev_lib_version, sizeof(unsigned short));
                        pos+= sizeof(unsigned short);
                    //memcpy(&ans.buff[pos], (char*)&HD->interface_version, sizeof(unsigned short));
                    memcpy(&ans.buff[pos], (char*)&desc.interface_version, sizeof(unsigned short));
                        pos+= sizeof(unsigned short);
                    //unsigned short sv = SERVER_VERSION;
                    unsigned short sv = 111;
                    memcpy(&ans.buff[pos], (char*)&sv, sizeof(unsigned short));
                        pos+= sizeof(unsigned short);
                    memcpy(&ans.buff[pos], (char*)&desc.len_dev_name, sizeof(unsigned int));
                        pos+= sizeof(unsigned int);
                    if (len_dev_name > 0)
                            // printf("desc.dev_name =%s \n", desc.dev_name);
                            // printf("len_dev_name =%d \n", len_dev_name);
                        //memcpy(&ans.buff[pos], &HD->dev_name[0], len);
                        memcpy(&ans.buff[pos], &desc.dev_name[0], len_dev_name);
                    }// от if
                    //ну и собственно заслать его даже если нулевой
                    //int cnt = send_packet(&ans);
                    int32_t len_send=sizeof(ans.command)+sizeof(ans.length) + ans.length; //структура ответа tcp_packet_t: command, length of buff //(ans.length)
                        // printf("len_send = %d\n", len_send);
                        // printf("strlen(ans.buff) = %d\n", strlen(ans.buff));
                    int8_t *TempMSV= new int8_t[len_send];
                    int32_t pos1=0;
                    memcpy(&TempMSV[pos1], &ans.command, sizeof(ans.command));
                        pos1+=sizeof(ans.command);
                    memcpy(&TempMSV[pos1], &ans.length, sizeof(ans.length));
                        pos1+=sizeof(ans.length);
                    memcpy(&TempMSV[pos1], &ans.buff[0], ans.length);
                    int32_t byteSend = port->clt_sock->send(&TempMSV[0], len_send);

                    if( byteSend > 0){
                         if(flag_get_log){send_log_message(port,"HAL_GET_DESC send", strlen("HAL_GET_DESC send"));}
                    }

                    delete[] ans.buff;    
                    delete[] TempMSV;

                break;
            }
            case CMD_HAL_START_LOG:{//complete //10
                    printf("Command: Start log task\n");
                flag_get_log=true;
                int res;
                /*if (log_task == NULL)
                log_task = new CLogTask(this);
                if (CHECK_EC(res = log_task->start())){
                printf("start log success\n");
                }
                else{
                printf("start log task error: %d\n", res);
                }*/

                //заслать ответ
                res=0;
                tcp_packet_t ans;
                memset(&ans, 0x00, sizeof(tcp_packet_t));
                ans.command = CMD_ANSWER;
                ans.length = sizeof(ans.command)+sizeof(res);
                ans.buff = new char[sizeof(packet.command)+sizeof(res)];
                memcpy(&ans.buff[0], (char*)&packet.command, sizeof(packet.command));
                memcpy(&ans.buff[sizeof(packet.command)], (char*)&res, sizeof(res));

                int32_t len_send=sizeof(ans.command)+sizeof(ans.length) + ans.length; //структура ответа tcp_packet_t: command, length of buff //(ans.length)
                int8_t *TempMSV= new int8_t[len_send];
                int32_t pos=0;
                memcpy(&TempMSV[pos], &ans.command, sizeof(ans.command));
                    pos+=sizeof(ans.command);
                memcpy(&TempMSV[pos], &ans.length, sizeof(ans.length));
                    pos+=sizeof(ans.length);
                memcpy(&TempMSV[pos], &ans.buff[0], ans.length);
                
                int32_t byteSend = port->clt_sock->send(&TempMSV[0], len_send);

                if(flag_get_log){send_log_message(port,"Logging was started", strlen("Logging was started"));}
                
                delete[] ans.buff;
                delete[] TempMSV;
                break;
            }
            case CMD_HAL_STOP_LOG:{//complete
                    printf("Command: Stop log task\n");
                int res = 0; //EC_SUCCESS
                // if (log_task != NULL){
                // res = log_task->stop();
                // delete log_task;
                // log_task = NULL;
                // }
                //заслать ответ
                tcp_packet_t ans;
                memset(&ans, 0x00, sizeof(tcp_packet_t));
                ans.command = CMD_ANSWER;
                ans.length = sizeof(ans.command)+sizeof(res);
                ans.buff = new char[sizeof(packet.command)+sizeof(res)];
                memcpy(&ans.buff[0], (char*)&packet.command, sizeof(packet.command));
                memcpy(&ans.buff[sizeof(packet.command)], (char*)&res, sizeof(res));

                int32_t len_send=sizeof(ans.command)+sizeof(ans.length) + ans.length; //структура ответа tcp_packet_t: command, length of buff //(ans.length)
                int8_t *TempMSV= new int8_t[len_send];
                int32_t pos=0;
                memcpy(&TempMSV[pos], &ans.command, sizeof(ans.command));
                    pos+=sizeof(ans.command);
                memcpy(&TempMSV[pos], &ans.length, sizeof(ans.length));
                    pos+=sizeof(ans.length);
                memcpy(&TempMSV[pos], &ans.buff[0], ans.length);
                
                int32_t byteSend = port->clt_sock->send(&TempMSV[0], len_send);

                if(flag_get_log){send_log_message(port,"Logging was stopped", strlen("Logging was stopped"));}

                flag_get_log=false;
                
                delete[] ans.buff;
                delete[] TempMSV;

                break;
            }
            case CMD_HAL_STOP_TS_TASK:{
                    printf("Command: stop TIMESIGNAL task\n");
                int res = TCP_EC_SUCCESS;
                // if (ts_task != NULL){
                // ts_task->stop();
                // }
                //заслать ответ
                ts_end_t ts_end;
                    memset(&ts_end, 0x00, sizeof(ts_end_t));
                    ts_end.ec = 100;
                    ts_end.adc_channel=200;
                    ts_end.sub_channel=300;
                    ts_end.freq_channel=400;
                    ts_end.all_samples=500;
                    ts_end.wrote_samples=600;
                    ts_end.wrote_labels=700;
                    ts_end.gain=1.0;
                    ts_end.gain_p3=2.0;
                
                tcp_packet_t ans;
                    memset(&ans, 0x00, sizeof(tcp_packet_t));
                    ans.command = CMD_ANSWER;
                
                //ans.command = CMD_ANSWER; //CALLBACK_TS_END 155
                ans.command = CALLBACK_TS_END; // 155
                ans.length = sizeof(ans.command)+sizeof(int); //+sizeof(res) ==8
                ans.buff = new char[ans.length];
                memcpy(&ans.buff[0], (char*)&ans.command, sizeof(ans.command));
                memcpy(&ans.buff[sizeof(ans.command)+sizeof(ans.command)], (char*)&res, sizeof(int));

                port->clt_sock->send((char*)&ans, sizeof(ans));
                delete[] ans.buff;
                break;
            }

            case 111:  
                    printf("case 111\n");  
                tcp_server_answer sServer_Answer;

                sServer_Answer.IDstm=100;
                sServer_Answer.in_command=0;
                sServer_Answer.out_command=1;
                while(flag_busy_port == true){}  //ждем когда можно отправлять, порт занят другой задачей
                if(flag_busy_port==false){
                    flag_busy_port=true;
                    port->clt_sock->send((char*)&sServer_Answer, sizeof(sServer_Answer));
                    flag_busy_port=false;
                }
                break;
            
            default:
                break;
            }
            delete[] packet.buff;
        }else{
            std::cout << "Element Not Found" << std::endl;      //такая задача не реализована
        }
    //===================================================================================================
 
  //  QueueTasks.pop();
//}
}

void parser_incoming_buffer_from_port(char* buf, int len){
    // union ULI{
    //     unsigned int UI;
    //     char Z[4];
    //     unsigned short US;
    // }UnionLI;

    // UnionLI.Z[0]=buf[0];
    // UnionLI.Z[1]=buf[1];
    // UnionLI.Z[2]=buf[2];
    // UnionLI.Z[3]=buf[3];

    // unsigned int uCommand= UnionLI.UI;
    // printf("uCommand = %u \n", uCommand);

    // unsigned int a = ((buf[0]-'0')*1000 + (buf[1]-'0')*100 +(buf[2]-'0')*10 +(buf[3]-'0'));
    // printf("u a = %u \n", a);

    //     printf("foo parser_incoming_buffer_from_port \n");
    //     printf("char* buf[0] = %d \n", buf[0]);
    if (!buf) return;
    //как минимум 2 по 4 байта (команда и длина)
    if (len < 8) return;
    tcp_packet_t packet;
    memset(&packet, 0x00, sizeof(tcp_packet_t));
    memcpy((char*)&packet.command, &buf[0], sizeof(packet.command));
    //memcpy((char*)&packet.command, &a, sizeof(packet.command));
    memcpy((char*)&packet.length, &buf[sizeof(packet.command)], sizeof(packet.length));
    if ((len-8) > 0){
        packet.buff = new char[len-8];
        memcpy(&packet.buff[0], &buf[sizeof(packet.command)+sizeof(packet.length)], len-8);
    }
        printf("parser packet.command = %u \n", packet.command);
        printf("parser packet.command = %d \n", packet.command);
        //printf("parser packet.command = %u \n", &packet.command);
        printf("=========================== \n");
    //QueueTasks.push(packet.command); //добавляем packet.command в очередь выполнения
    //dispatcher2(QueueTasks, packet);
}


void call_getCommandPortThread(CreatePort *port){
        printf("foo getCommandPortT\n");
    string strRecv_msv;
    int32_t sizeReceivedMsv=100;
    //char ReceivedMsv[100] = {0, };               /* buffer for command from port */
    //char* ReceivedMsv=" ";
    while(1){
        if(flagAllowRecvFromPort){
            char *ReceivedMsv = new char[sizeReceivedMsv]{0};               /* buffer for command from port */
            //char ReceivedMsv2[100] = " ";
            //ReceivedMsv2[0]=100;
             //ReceivedMsv2[2]=50;
             //printf("strlen ReceivedMsv2 = %d\n", strlen(ReceivedMsv2));

        // for(int i=0; i<strlen(ReceivedMsv2); ++i){
        //     printf("ReceivedMsv2[%d] = %d \n", i, ReceivedMsv2[i]);
        // }
            //int *ReceivedMsv2 = new int[100]{0};               /* buffer for command from port */
            //port->clt_sock->recv(ReceivedMsv, sizeReceivedMsv);
            port->clt_sock->recv(ReceivedMsv, 100);
            //parser_incoming_buffer_from_port(ReceivedMsv, strlen(ReceivedMsv));

                //printf("ReceivedMsv port80 = %s \n", ReceivedMsv);
                //printf("strlen ReceivedMsv80 = %d\n", strlen(ReceivedMsv));

            strRecv_msv=ReceivedMsv;
                //getCommandFromPort3(port, strRecv_msv);
            QueueTasks.push(strRecv_msv);

            //getCommandFromPort3(strRecv_msv);
            //dispatcher(port, ReceivedMsv2, strlen(ReceivedMsv2));
            dispatcher(port, ReceivedMsv, sizeReceivedMsv);
            //getCommandFromPort3(QueueTasks);
            delete[] ReceivedMsv;
            //delete[] ReceivedMsv2;
        }

    }
}



int main()
{
        printf("\n======== 1-start ======================\n");  fflush(stdout);
    //===================================================================================================
    for( int32_t i=2; i<16; ++i){
       CMD_ARRAY.push_back(i);
    }
    CMD_ARRAY.push_back(17);
    CMD_ARRAY.push_back(18);
    CMD_ARRAY.push_back(19);
    CMD_ARRAY.push_back(43);
    CMD_ARRAY.push_back(44);
    CMD_ARRAY.push_back(111);
        // for( iter=CMD_ARRAY.begin(); iter<CMD_ARRAY.end(); ++iter){ //вывод в консоль для отладки
        //     std::cout << *iter <<std::endl;
        // }
    //===================================================================================================== 

    #ifdef MBED_MAJOR_VERSION
        printf("Mbed OS version: %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    #endif

    spi.format(8,3);        // Setup:  bit data, high steady state clock, 2nd edge capture
    spi.frequency(1000000); //1MHz
    int resEthInit = ethernetInterfaceInit();


    

    //CreatePort *PortConnect = new CreatePort(eth, 80);
    //if(resEthInit==0 && PortConnect->flagReadyPort==true){
    if(resEthInit==0){
        CreatePort *PortConnect = new CreatePort(eth, 80);
            if(PortConnect->flagReadyPort==true){  //только когда есть соединение с клиентом, тогда имеет смысл снимать сигнад и тд
                drdyIN.rise(&drdyINHandlerRise);   //interrupt DRDY flag from slave (hardware)
                spiThread.start(call_spiThread2);  //get data SPI from Slave equipment
                getCommandPortThread.start(callback(call_getCommandPortThread, PortConnect));   //get command from general port
                timeSignalPortThread.start(callback(call_timeSignalPortThread, PortConnect));   //send time Signal in ethernet port to client
                // if(flag_cmd_check_answer){
                //     printf("flag_cmd_check_answer\n");
                //     f_cmd_check_connection(PortConnect);
                //     flag_cmd_check_answer=false;
                // }
                
            }
    }else{printf("Ethernet NO CONNECT\n");  fflush(stdout);}




        //generalPortThread.start(call_generalPortThread);            //get command from general port
        //timeSignalPortThread.start(call_timeSignalPortThread);      //send time Signal in ethernet own separate port to client
    


    
     /*
    if(resEthInit<0){printf("Ethernet NO CONNECT\n");  fflush(stdout);}
    else{  
        drdyIN.rise(&drdyINHandlerRise);   //interrupt DRDY flag from slave (hardware)
        spiThread.start(call_spiThread2);  //get data SPI from Slave equipment

        generalPortThread.start(call_generalPortThread);            //get command from general port
        timeSignalPortThread.start(call_timeSignalPortThread);      //send time Signal in ethernet own separate port to client
    }
    */
    while(1){
        // printf("1111\n");
        // printf("flag_cmd_check_answer = %d\n",flag_cmd_check_answer);
        //         if(flag_cmd_check_answer){
        //             printf("flag_cmd_check_answer\n");
        //             f_cmd_check_connection(PortConnect);
        //             flag_cmd_check_answer=false;
        //         }
    }
    //delete PortConnect;
}

    //std::vector<int>::difference_type index = std::distance(CMD_ARRAY.begin(), iter);

    //iter = std::find(CMD_ARRAY.begin(), CMD_ARRAY.end(), 22);

                        // for(int32_t i=0; i<len_send; ++i){
                        //     printf("TempMSV[%d] = %d\n", i, TempMSV[i]);
                        // }

// unsigned int x = 1001;
// char y;
// printf("x & 0xFF = %d \n", x & 0xFF);
// printf("(x >> 8) & 0xFF = %d \n", (x >> 8) & 0xFF);

//     for(int i=0; i<(sizeof(MsvTemp)/sizeof(MsvTemp[0])); ++i){
//         printf("MsvTemp[%d] = %d \n", i, MsvTemp[i]);
//     }


                    
                        // printf("ans.command = %d\n", ans.command);
                        // printf("ans.length = %d\n", ans.length);

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

/*
void getCommandFromPort(char* ptr_recv_msv)
{
    int32_t valueFromCommand = ptr_recv_msv[0]-0x30;
   // обнулить входящий массив после того как он отработает

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
*/