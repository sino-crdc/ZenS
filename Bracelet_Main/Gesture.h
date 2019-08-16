#ifndef Gesture_H
#define Gesture_H
#if defined(ARDUINO)&&ARDUINO>=100
  #include "Arduino.h"
#else 
  #include "WProgram.h"
#endif

#include <SD.h>
#include "Setting.h"
#include "Device.h"
#include "Gest_Quantity_Data.h"
#include "Order.h"
#include "Controler.h"
#include <math.h>
#include <SoftwareSerial.h>

class Gesture{
 public:
    Gesture(Device* device);
    static void initial();
    void setGest_data(Gest_Data* gest_data);
    Gest_Data* getGest_data();
    Gest_Data* detect();
    Order* analyze(Gest_Data* gest_data);
    Gest_Quantity_Data* quantity_detect(Order* order);
    Order* quantity_analyze(Gest_Quantity_Data* gest_quantity_data);

  private:
    Device* device;
    Gest_Data* gest_data = NULL;
    
    void serialEvent(); 
};

#endif
