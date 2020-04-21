#include "writereadflash.h"

WriteReadFlash::WriteReadFlash(): mbed::FlashIAP()
{

}

int32_t WriteReadFlash::flash_write_data(char *page_buffer, uint32_t address){
        //flash.init();
        init();

    //const uint32_t page_size = flash.get_page_size();
    const uint32_t page_size = get_page_size();
        printf("page_size =  %u\n", page_size);  fflush(stdout);
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
        int32_t resultWriteFlash= program(page_buffer, addr, page_size);
        //if(resultWriteFlash < 0){ return -2;}
            //flash.program(page_buffer2, addr, page_size);

        addr += page_size;
        if (addr >= next_sector) {
            //next_sector = addr + flash.get_sector_size(addr);
            next_sector = addr + get_sector_size(addr);
            sector_erased = false;
        }
            printf("Flashed 100%%\r\n");
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