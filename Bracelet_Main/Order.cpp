#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Order.h"

union xx{               //用于读取文件中的数据；
    unsigned int num=0;
    byte buf[2];
};

Order::Order(Device* device, String orderType)
{
  this->device = device;
  this->orderType = orderType;
  setIsQuantity();
  Serial.println("order constructed.");
  
}
void Order::setDevice(Device* device)
{
  Serial.println("Order::setDevice");
  this->device = device;
}
void Order::setOrderType(String orderType)
{
  Serial.println("Order::setOrderType");
  this->orderType = orderType;
}
void Order::setCode()
{
  if(this->coding.buf[0] != 0){//判断Order对象的Coding有没有set
    return;
  }
  Serial.println("Order::setCode");
  if (this->isQuantity) {
    Code* codings = this->device->getCodings();
    String* orderTypes =this->device->getOrderTypes();
    int orderNum = this->device->getOrderNum();
    for (int i = 0; i < orderNum; i++) {
      if (this->orderType.equals(orderTypes[i])) {
        this->coding = codings[i];
        break;
      }
    }
    Serial.println("Order::setCode isQuantity finish");
  } else {
    if (SD.begin(SD_PIN)) {
      File file = SD.open(this->device->getInfos().name(),FILE_READ);
      if (file) {
        for (int i = 0; file.available() && i < this->device->getOrderNum(); i++) {

          //获取第i个orderType
          String data = "";
          while (char(file.peek()) != 0xFF) {
            data += char(file.read());
          }
          //比对orderType
          if (data.equals(this->orderType)) { //如果匹配成功，加载code并退出
            file.read();//丢弃0xFF
            union xx code;
            unsigned int * int_coding_o = new unsigned int[243];
            int len=0; 
            for(int i=0;i<243;i++)
              int_coding_o[i]=0;
            while(file.available()){
                code.buf[1]=file.read();
                code.buf[0]=file.read();
                if(code.buf[0]==0xFF||code.buf[1]==0xFF)
                  break;
                else
                  int_coding_o[len++]=code.num;
              }
//            Serial.println(len);
            for(int i=0;i<len;i++){
              this->coding.buf[i]=int_coding_o[i];
//              Serial.print(int_coding_o[i]),Serial.print(" ");
            }
//            Serial.println();
            //int* temp_p = int_coding_o;
            //int* int_coding_o = (int*)by_coding_o;
            this->coding.len = len;
            this->coding.hz = 38;
            delete [] int_coding_o;
            break;
          } else { //如果匹配失败，则继续读到\n后
            while (file.available() && char(file.read()) != '\n');
          }
        }
      }
      file.close();
      Serial.println("Order::setCode not isQuantity finish");
    }
  }
}
void Order::setIsQuantity()
{  
  //Serial.println("Order::setIsQuantity");
  if (this->orderType.charAt(0) == '$')
    this->isQuantity = true;
  else
    this->isQuantity = false;
}
Device* Order::getDevice()
{
 // Serial.println("Order::getDevice");
  return this->device;
}
String Order::getOrderType()
{
  //Serial.println("Order::getOrderType "+orderType);
  return this->orderType;
}
Code Order::getCode()
{
  //Serial.println("Order::getCode");
  setCode();
  return this->coding;
}
bool Order::getIsQuantity()
{
  //Serial.println("Order::getIsQuantity "+isQuantity);
  return this->isQuantity;
}
