#include "createthread.h"

CreateThread::CreateThread() : rtos::Thread(osPriorityNormal, 1024)
{
    
}

void CreateThread::call_thread()
{
    while(1){

    }
}
void CreateThread::createThread(){
    //Thread t1;
    t1.start(call_thread);
}