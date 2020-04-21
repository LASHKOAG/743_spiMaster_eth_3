#ifndef BUTTONCLASS_H
#define BUTTONCLASS_H

#include "mbed.h"

class ButtonClass
{
public:
    ButtonClass(PinName, PinMode);
    ~ButtonClass();

    DigitalIn *btn_in;
    Timer debounceBtn;

    int32_t check_onPressed_btn();
};

#endif // BUTTONCLASS_H
