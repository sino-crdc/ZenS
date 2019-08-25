#ifndef Controler_H
#define Controler_H
#if defined(ARDUINO)&&ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Setting.h"
#include "Order.h"
#include <IRremote2.h>

class Controler {
  private:
    bool buttonA = false;
    bool buttonB = false;
    bool buttonC = false;
    bool buttonD = false;
  
  public:
    Controler();
    void setButtonA(bool state);
    void setButtonB(bool state);
    void setButtonC(bool state);
    void setButtonD(bool state);
    bool getButtonA();
    bool getButtonB();
    bool getButtonC();
    bool getButtonD();
    void initial();
    bool isPressing();
    Device* device();
    void send(Order* order);
    void terminate();
    byte detect();
};

#endif
