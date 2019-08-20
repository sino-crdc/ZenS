#ifndef Order_H
#define Order_H
#if defined(ARDUINO)&&ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Setting.h"
#include "Device.h"
#include <SD.h>

class Order {
  public:
    Order(Device* device, String orderType);
    void setDevice(Device* device);
    void setOrderType(String orderType);
    void setCode();
    void setIsQuantity();
    Device* getDevice();
    String getOrderType();
    Code getCode();
    bool getIsQuantity();

  private:
    Device* device;
    String orderType;
    Code coding;
    bool isQuantity;

    
};
#endif
