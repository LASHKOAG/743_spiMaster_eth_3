#ifndef CIRCBUFF_H
#define CIRCBUFF_H

#include <stdio.h>

class CircBuff
{
public:
    CircBuff();
    ~CircBuff();

    int readIndex;
    int writeIndex;
    bool isEmpty;
    bool isFull;
    bool goToStartWriteIndex;
    bool goToStartReadIndex;

    void initStructCircularBuffer();
    int putCircularBuffer(char* ptrMainBuffer, int ptrSIZE_MAINBUFFER, char aValue);
    char getCircularBuffer(char* ptrMainBuffer, int ptrSIZE_MAINBUFFER);
    void clearCircularBuffer();
    int isEmptyCircularBuffer();
    int isFullCircularBuffer();

};

#endif // CIRCBUFF_H
