#ifndef Gest_Quantity_Data_H
#define Gest_Quantity_Data_H
#if defined(ARDUINO)&&ARDUINO>=100
  #include "Arduino.h"
#else 
  #include "WProgram.h"
#endif

#include "Setting.h"
#include "Device.h"
#include "Gest_Data.h"
#include "Order.h"
#include <stdlib.h>

class Gest_Data;
class Order;
class Gest_Quantity_Data: public Gest_Data{
  public:
    //String equation;
    //Device* device;
    float angle;
    Order* order;
    const char axe = QUANTITY_AXE;
    Gest_Quantity_Data(float angle,Order* order,Device* device);
};
#endif
