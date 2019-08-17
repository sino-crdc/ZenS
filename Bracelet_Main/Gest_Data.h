#ifndef Gest_Data_H
#define Gest_Data_H
#if defined(ARDUINO)&&ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Device;

class Gest_Data {
  public:
    String equation;
    Device *device;

    Gest_Data(String equation, Device* device);
    Gest_Data(){
      Gest_Data("", NULL);
    }
};
#endif
