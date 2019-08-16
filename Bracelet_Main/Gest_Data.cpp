#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Gest_Data.h"

Gest_Data::Gest_Data(String equation, Device* device) {
  this->equation = equation;
  this->device = device;
}
