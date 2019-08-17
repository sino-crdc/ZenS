#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Gest_Quantity_Data.h"

Gest_Quantity_Data::Gest_Quantity_Data(float angle,Order* order,Device* device):Gest_Data("",device){
  Serial.println("gest_quantity_data constructing...");
  this->order = order;
  this->angle = angle;
  Serial.print("gest_quantity_data constructed: ");
  Serial.println("order^"+this->order->getDevice()->getName()+"^"+this->order->getOrderType()+" angle^"+this->angle);
} 
