#ifndef WRITEREADFLASH_H
#define WRITEREADFLASH_H

#include "mbed.h"

class WriteReadFlash:FlashIAP
{
public:
    WriteReadFlash();
    ~WriteReadFlash();

    DigitalIn *btn_in;
    Timer debounceBtn;
    Thread testThread;
    
    FlashIAP flash; //чтобы работал метод read() класса FlashIAP
    
    int32_t flash_write_data(char *page_buffer, uint32_t address);
    uint32_t flash_read_2(uint32_t address);  //2-ый способ чтения flash;
    //1 ый способ вызвать read() FlashIAP
};

#endif // WRITEREADFLASH_H