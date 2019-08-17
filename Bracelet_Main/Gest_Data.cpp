#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Gest_Data.h"

Gest_Data::Gest_Data(String equation, Device* device) {
  this->equation = equation;
  this->device = device;
  //invalid use of incomplete type: device. 可能并不知道初始空间大小的配置等
}
