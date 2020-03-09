#include "circbuff.h"

CircBuff::CircBuff()
{
    initStructCircularBuffer();
}


CircBuff::~CircBuff() {};

void CircBuff::initStructCircularBuffer()
{
    readIndex  = 0;
    writeIndex = 0;
    isEmpty    = 1;
    isFull     = 0;
}
//------------------------------------------------------------------------------
int CircBuff::putCircularBuffer(char* ptrMainBuffer, int ptrSIZE_MAINBUFFER, char aValue)
{
    if(isFull) {
        return -1;   /*если буффер полон*/
    }
    if(writeIndex >= ptrSIZE_MAINBUFFER) {
        writeIndex = 0;   /*переместиться в самое начало*/
    }
    if(isEmpty) { /*если буффер пуст*/
        isEmpty = 0;
        // data[writeIndex++] = aValue;
        ptrMainBuffer[writeIndex++] = aValue;

        if (writeIndex == readIndex)/*один индекс догнал другой, буффер полон*/
            isFull  = 1;
        return 1;
    }

    ptrMainBuffer[writeIndex++] = aValue; /*если буффер не пуст*/

    if (writeIndex == readIndex) {
        isFull  = 1;
    }

    return 1;
}
//------------------------------------------------------------------------------

char CircBuff::getCircularBuffer(char* ptrMainBuffer, int ptrSIZE_MAINBUFFER)
{
    if(isEmpty) {
        return -1;   /*если буффер пуст*/
    }

    isFull = 0;  /*??? if ?? если буффер не пуст*/

    if(readIndex >= ptrSIZE_MAINBUFFER) {
        readIndex = 0;
    }

    char res = ptrMainBuffer[readIndex++];
    //while((readIndex + 1) == writeIndex){;}

    if((readIndex + 1) == writeIndex) {
        /*В самом начале скорость чтения больше скорости записи буффера.
         Это приведет к ошибочному сбору данных.
         Данная проверка "притормаживает" read из буффера.*/
        //test1=true;
        //wait_ms(100000);
        //printf("bool test1=%d \n",test1);fflush(stdout); /*хорошо работает вместо wait. Read to Eth is more faster than writing SPI*/
        printf("bool test1=%d \n", 1);
        fflush(stdout);
        //test1=false;
        //for(uint32_t i =0; i<4000000000; ++i){;}
    }

    if(readIndex == writeIndex) {
        isEmpty = 1;
    }

    return  res;
}
//------------------------------------------------------------------------------

void CircBuff::clearCircularBuffer()
{
    isEmpty    = 1;
    isFull     = 0;
    writeIndex = 0;
    readIndex  = 0;
}
//------------------------------------------------------------------------------

int CircBuff::isEmptyCircularBuffer()
{
    return isEmpty;
}
//------------------------------------------------------------------------------
int CircBuff::isFullCircularBuffer()
{
    return isFull;
}
//------------------------------------------------------------------------------
