#ifndef WRITEREADFLASH_H
#define WRITEREADFLASH_H

#include "mbed.h"
#include "defines.h"
// #include <vector>
// #include <string>

//#include "socket_struct.h"

class WriteReadFlash:FlashIAP
{
public:
    WriteReadFlash();
    ~WriteReadFlash();

    DigitalIn *btn_in;
    Timer debounceBtn;
    Thread testThread;
    
    FlashIAP flash; //чтобы работал метод read() класса FlashIAP
    
    int32_t flash_write_data(char *page_buffer, uint32_t address, uint32_t sizeOfMessage );
    uint32_t flash_read_2(uint32_t address);  //2-ый способ чтения flash;
    //1 ый способ вызвать read() FlashIAP

   //int32_t get_info_from_flash(char*, uint32_t sizeOfMessage);

    //int32_t flash_write_data(char *page_buffer, uint32_t address, uint32_t sizeOfMessage );
    
    // typedef struct{
    //     char ip[14];
    //     char mask[14];
    //     char gateway[14];
    // }_test_flash;

    // ts_begin_t beginT;
    

    //_test_flash sTest_flash;

    //int32_t get_copy_flash_userSpace(char*, uint32_t addressFrom, uint32_t addressFromSize,  uint32_t addressTo, uint32_t addressToSize);
    int32_t get_copy_flash_userSpace();

    //int32_t get_convert_struct_to_massive();

    int32_t get_data_flash_userSpace(char*, int32_t);
    
};

#endif // WRITEREADFLASH_H