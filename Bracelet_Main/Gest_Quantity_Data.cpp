#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Gest_Quantity_Data.h"

Gest_Quantity_Data::Gest_Quantity_Data(float angle,Order* order,Device* device):Gest_Data("",device){
  this->order = order;
  this->angle = angle;
} 
