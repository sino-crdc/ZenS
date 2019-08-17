#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Order.h"

Order::Order(Device* device, String orderType)
{
  this->device = device;
  this->orderType = orderType;
  setIsQuantity();
}
void Order::setDevice(Device* device)
{
  this->device = device;
}
void Order::setOrderType(String orderType)
{
  this->orderType = orderType;
}
void Order::setCode()
{
  if (this->isQuantity) {
    Code* codings = this->device->getCodings();
    String* orderTypes = this->device->getOrderTypes();
    int orderNum = this->device->getOrderNum();
    for (int i = 0; i < orderNum; i++) {
      if (this->orderType.equals(orderTypes[i])) {
        this->coding = codings[i];
        break;
      }
    }
  } else {
    if (SD.begin(SD_PIN)) {
      File file = SD.open(this->device->getInfos().name(),FILE_READ);
      if (file) {
        for (int i = 0; file.available() && i < this->device->getOrderNum(); i++) {

          //获取第i个orderType
          String data = "";
          while (char(file.peek()) != '#') {
            data += char(file.read());
          }
          //比对orderType
          if (data.equals(this->orderType)) { //如果匹配成功，加载code并退出
            file.read();//丢弃'#'

            char* by_coding_o = new char[972];
            for (int j = 0; char(file.peek()) != '#'; j++) {
              by_coding_o[j] = char(file.read());
            }

            int* int_coding_o = (int*)by_coding_o;
            
            for(int j = 0;j < 243; j++){
              this->coding.buf[j] = int_coding_o[j];
            }
            
            this->coding.len = 243;
            this->coding.hz = 38;
            delete [] by_coding_o;
            break;
          } else { //如果匹配失败，则继续读到\n后
            while (file.available() && char(file.read()) != '\n');
          }
        }
      }
      file.close();
    }
  }
}
void Order::setIsQuantity()
{  
  if (this->orderType.charAt(0) == '$')
    this->isQuantity = true;
  else
    this->isQuantity = false;
}
Device* Order::getDevice()
{
  return this->device;
}
String Order::getOrderType()
{
  return this->orderType;
}
Code Order::getCode()
{
  setCode();
  return this->coding;
}
bool Order::getIsQuantity()
{
  return this->isQuantity;
}
