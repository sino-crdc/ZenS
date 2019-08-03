#ifndef Controler_H
#define Controler_H
#if defined(ARDUINO)&&ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Setting.h"
#include "Order.h"
#include <IRremote.h>

class Controler {
//  private:
//    static bool buttonA;
//    static bool buttonB;
//    static bool buttonC;
//    static bool buttonD;
  
  public:
    static void setButtonA(bool state);
    static void setButtonB(bool state);
    static void setButtonC(bool state);
    static void setButtonD(bool state);
    static bool getButtonA();
    static bool getButtonB();
    static bool getButtonC();
    static bool getButtonD();
    static void initial();
    static bool isPressing();
    static Device* device();
    static void send(Order* order);
    static void terminate();
    static byte detect();
};

//bool Controler::buttonA = false;
//bool Controler::buttonB = false;
//bool Controler::buttonC = false;
//bool Controler::buttonD = false;

#endif
