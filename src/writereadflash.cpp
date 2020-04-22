#include "writereadflash.h"

WriteReadFlash::WriteReadFlash(): mbed::FlashIAP()
{
    // sTest_flash.ip = "192.168.4.177";
    // sTest_flash.mask = "255.255.255.0";
    // sTest_flash.gateway = "192.168.4.1";
        //set_network("192.168.4.177","255.255.255.0","192.168.4.1");  
}

int32_t WriteReadFlash::flash_write_data(char *page_buffer, uint32_t address, uint32_t sizeOfMessage){
        //flash.init();
        uint32_t page_count=0;
        const uint32_t page_size = get_page_size();
            //printf("page_size =  %u\n", page_size);  fflush(stdout);
        if(sizeOfMessage>=page_size){
            page_count = (sizeOfMessage / page_size);
            if((sizeOfMessage % page_size>0)){page_count+=1;}
                //printf("page_count =  %u\n", page_count);  fflush(stdout);
        }else{
            page_count=1;
        }
        init();

    //const uint32_t page_size = flash.get_page_size();
    //const uint32_t page_size = get_page_size();
        // printf("page_size =  %u\n", page_size);  fflush(stdout);
    //char *page_buffer = new char[page_size];
            //memset(page_buffer, 0, sizeof(char) * page_size);
            // page_buffer[0]=125; //for test
            // page_buffer[1]=100; //for test
            // page_buffer[2]=50; //for test

            //page_buffer[0]='A'; //for test
            //page_buffer[1]='B'; //for test
            //page_buffer[2]='C'; //for test

            //char page_buffer2[]="192.168.4.177"; //for test

    uint32_t addr = address;
    //uint32_t next_sector = addr + flash.get_sector_size(addr);
    uint32_t next_sector = addr + get_sector_size(addr);
    bool sector_erased = false;
    size_t pages_flashed = 0;
    uint32_t percent_done = 0;
    uint32_t countBytes = 0;

    while(page_count--){
            //while (true) {
        // Erase this page if it hasn't been erased
        if (!sector_erased) {
            //int32_t resultEraseFlash = flash.erase(addr, flash.get_sector_size(addr));
            int32_t resultEraseFlash = erase(addr, get_sector_size(addr));
                //printf("resultEraseFlash = %d\n", resultEraseFlash);
            //if(resultEraseFlash < 0){ return -1;}//так делать не надо  
            sector_erased = true;
        }

        // Program page
        //int32_t resultWriteFlash= flash.program(page_buffer, addr, page_size);
        int32_t resultWriteFlash= program(&page_buffer[countBytes], addr, page_size);
        //if(resultWriteFlash < 0){ return -2;}
            //flash.program(page_buffer2, addr, page_size);

        addr += page_size;
        if (addr >= next_sector) {
            //next_sector = addr + flash.get_sector_size(addr);
            next_sector = addr + get_sector_size(addr);
            sector_erased = false;
        }
        countBytes+=page_size;
    }
    countBytes=0;
            //printf("Flashed 100%%\r\n");
    //flash.deinit();
    deinit();

    return 0;
    /*example for to use
        //main
        char buf[14]="120.111.4.222";
        int32_t res = flash_write_data(&buf[0], ADDRESS_MEMORY);  //0x08038500  //средствами mbed os
        printf("res = %d\n",res);
        //char buf2[strlen("120.111.4.222")+1]="";
        //char* buf2="";
        char buf2[14];
        flash.read(buf2, ADDRESS_MEMORY, strlen("120.111.4.222"));
            printf("buf2 s = %s\n",buf2);
        for(int i=0; i<strlen("120.111.4.222"); ++i){
            printf("buf2[%d] d = %d\n",i, buf2[i]);
            printf("buf2[%d] c = %c\n",i, buf2[i]);
        }


        char tt = flash_read_2(ADDRESS_MEMORY);// 2ой способ
            printf("tt = %u\n",tt);
        char tt1 = flash_read_2(ADDRESS_MEMORY);
            printf("tt1 = %u\n",tt1);

        char testBuff[14]="";
        testBuff[0]=flash_read_2(ADDRESS_MEMORY);
        testBuff[1]=flash_read_2(ADDRESS_MEMORY+1);
        testBuff[2]=flash_read_2(0x08038502);
        testBuff[3]=flash_read_2(0x08038503);
            printf("testBuff = %s\n",testBuff);
*/
}
    
uint32_t WriteReadFlash::flash_read_2(uint32_t address) {  //2-ый способ чтения flash; 
    return (*(__IO uint32_t*) address);
}

int32_t WriteReadFlash::get_copy_flash_userSpace(){
    //взять данные с адреса-1 длиной N байт
    //записать в адрес-2 длиной N байт
    //адрес-1 скрыт: ADDRESS_MEMORY_DEVELOPER. Адрес-2 скрыт: ADDRESS_MEMORY_USER  //defines.h
    //длина N байт одинакова:  SIZE_INFO_FLASH  //defines.h

    // #define ADDRESS_MEMORY_DEVELOPER 0x08038400
    // #define ADDRESS_MEMORY_USER 0x08038500
    // #define SIZE_INFO_FLASH 64 //bytes //2 страницы, пишется постранично

    char *page_buffer = new char[SIZE_INFO_FLASH];
    if(page_buffer==NULL){return -1;} //память не выделилась
    memset(page_buffer, 0, SIZE_INFO_FLASH);

    flash.read(page_buffer, ADDRESS_MEMORY_DEVELOPER, SIZE_INFO_FLASH); //взяли заводские значания
        printf("page_buffer =  %s\n", page_buffer);  fflush(stdout);

        // for(int i=0; i<strlen(page_buffer); ++i){
        //     //printf("page_buffer[%d] d = %d\n",i, page_buffer[i]);
        //     printf("page_buffer[%d] c = %c\n",i, page_buffer[i]);
        // }
    
    flash_write_data(page_buffer, ADDRESS_MEMORY_USER, SIZE_INFO_FLASH); //сделали копию для пользователя
        printf("====copy info flash to flash have done ! =======================\n");  fflush(stdout);

    if(page_buffer!=NULL){delete[] page_buffer;}
    return 0;
}

// int32_t WriteReadFlash::get_convert_struct_to_massive(){

// }

int32_t WriteReadFlash::get_data_flash_userSpace(char*, int32_t){
    //с адреса userSpace считать массив
    //разложить в структуру
    //
    return 0;
}
// int32_t get_info_from_flash(char* TempMsv, uint32_t sizeOfMessage){
//     // while(*TempMsv != ';'){
//     //     TempMsv++;
//     // }

//     while(sizeOfMessage--){
//         TempMsv++;
//     }
//     return 0;
// } 