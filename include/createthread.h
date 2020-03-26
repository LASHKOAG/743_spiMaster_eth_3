#ifndef CREATETHREAD_H
#define CREATETHREAD_H

#include "mbed.h"

class CreateThread : public rtos::Thread
{
public:
    CreateThread();
    Thread t1;
    void call_thread();
    void createThread();
};

#endif // CREATETHREAD_H
