#include "buttonclass.h"

ButtonClass::ButtonClass(PinName _PinName, PinMode _PinMode)
{
    btn_in = new DigitalIn(_PinName);       // пин
    btn_in->mode(_PinMode);                 // подтяжка
}

int32_t ButtonClass::check_onPressed_btn()
{
    if (btn_in->read()==0) {                //  ==0 так как подтянут к PullUp
        debounceBtn.start();                //используем таймер
        if(debounceBtn.read_ms() > 2500) {  //500  
            debounceBtn.reset();            //сброс таймера
            return 0;
        }
    }
    return -1;
}

ButtonClass::~ButtonClass()
{
    delete btn_in;
}
