#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Device.h"

//todo 设置机制，将SD卡中存储数据，有用的或者频繁使用的部分加载到EEPROM
Device::Device(String name, File infos,int orderNum){
  this->name = name;
  this->infos = infos;
  this->orderNum = orderNum;
  //Serial.println("device constructed.");
}

String Device::getName(){
//  Serial.println("Device::getname");
  return this->name;
}

File Device::getInfos(){
//  Serial.println("Device::getInfos");
  return this->infos;
}

int Device::getOrderNum(){
//  Serial.println("Device::getOrderNum");
  return this->orderNum;
}

String* Device::getOrderTypes(){
//  Serial.println("Device::getOrderTypes");
  if(this->orderTypes == NULL){
    complete();
  }
  return this->orderTypes;
}

Code* Device::getCodings(){
//  Serial.println("Device::getCodings");
  if(this->codings == NULL){
    complete();
  }
  return this->codings;
}

Gest_Data* Device::getGestures(){
//  Serial.println("Device::getGestures");
  if(this->gestures == NULL){
    complete();
  }
  return this->gestures;
}

void Device::complete(){
  Serial.println("Device::complete");
  if(SD.begin(SD_PIN)){
    if(SD.open(this->infos.name(),FILE_READ)){
      const int num = this->orderNum;
      this->orderTypes = new String[num];
      this->codings = new Code[num];
      this->gestures = new Gest_Data[num];
      for(int i = 0;this->infos.available() && i<orderNum;i++){
        Serial.println("for "+String(i))
        //获取第i个orderType
        String data = "";
        Serial.println(this->infos.peek(), HEX);
        while(char(this->infos.peek()) != 0xFF){
          data += char(this->infos.read());
        }
        this->orderTypes[i] = data;
        this->infos.read();//丢弃0xFF
        Serial.println(this->orderTypes[i]);

        //获取第i个coding
        char* by_coding = new char[972];
        for(int j = 0;char(this->infos.peek()) != 0xFF;j++){
          by_coding[j] = char(this->infos.read());
        }

        int* int_coding = (int*)by_coding;
        
        for(int j = 0;j < 243; j++){
          this->codings[i].buf[j] = int_coding[j];  
        }
        
        this->codings[i].len = 243;
        this->codings[i].hz = 38;
        delete [] by_coding;
        this->infos.read();
        Serial.println(this->codings[i].len);
        //获取第i个gesture
        String temp = "";
        while(char(this->infos.peek()) != '\r'){
          temp += char(this->infos.read());
        }
        this->gestures[i].equation = temp;
        this->gestures[i].device = this;
        this->infos.read();//抛弃\r
        this->infos.read();//抛弃\n
        Serial.println(this->gestures[i].equation);
      }
    }
    this->infos.close();
    Serial.println("Device::complete finish");
  }
}
