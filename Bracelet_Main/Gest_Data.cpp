#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Gest_Data.h"
#include "Device.h"

Gest_Data::Gest_Data(String equation, Device* device) {
//  Serial.println("gestdata constructing...");
  this->equation = equation;
  this->device = device;
//  Serial.print("gestdata constructed: ");
  //invalid use of incomplete type: device. 可能并不知道初始空间大小的配置等
  if(this->device == NULL){
    Serial.println("equation^"+this->equation+" device^NULL");
  }else{
    Serial.println("equation^"+this->equation+" device^"+this->device->getName());
  }
  //todo 修改没有上传到 master
}
