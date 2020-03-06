#ifndef HWCLOCKSTM_H
#define HWCLOCKSTM_H

#include "mbed.h"
#include <string>
#include <iostream>

void write_rtc(std::string& input, int utc);
void write_rtc2(time_t t, int utc);
void read_rtc();

/*

class hwclockstm
{
public:
    hwclockstm();
};
*/
#endif // HWCLOCKSTM_H
