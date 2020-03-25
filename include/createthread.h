#ifndef CREATETHREAD_H
#define CREATETHREAD_H

#include "mbed.h"

class CreateThread : public Thread
{
public:
    CreateThread();
    void call_thread();
};

#endif // CREATETHREAD_H
