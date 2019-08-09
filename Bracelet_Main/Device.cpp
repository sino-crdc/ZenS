#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Device.h"

//todo 设置机制，将SD卡中存储数据，有用的或者频繁使用的部分加载到EEPROM
Device::Device(String name, IRsend irsend, File infos,int orderNum){
  this->name = name;
  this->irsend = irsend;
  this->infos = infos;
  this->orderNum = orderNum;
}

String Device::getName(){
  return this->name;
}

IRsend Device::getIrsend(){
  return this->irsend;
}

File Device::getInfos(){
  return this->infos;
}

int Device::getOrderNum(){
  return this->orderNum;
}

String* Device::getOrderTypes(){
  if(this->orderTypes == NULL){
    complete();
  }
  return this->orderTypes;
}

Code* Device::getCodings(){
  if(this->codings == NULL){
    complete();
  }
  return this->codings;
}

Gest_Data* Device::getGestures(){
  if(this->gestures == NULL){
    complete();
  }
  return this->gestures;
}

void Device::complete(){
  if(SD.begin(SD_PIN)){
    if(SD.open(this->infos.name(),FILE_READ)){
      const int num = this->orderNum;
      this->orderTypes = new String[num];
      this->codings = new Code[num];
      this->gestures = new Gest_Data[num];
      for(int i = 0;this->infos.available() && i<orderNum;i++){
        
        //获取第i个orderType
        String data = "";
        while(char(this->infos.peek()) != '#'){
          data += char(this->infos.read());
        }
        this->orderTypes[i] = data;
        this->infos.read();//丢弃'#'

        //获取第i个coding
        char* by_coding = new char[972];
        for(int j = 0;char(this->infos.peek()) != '#';j++){
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
         
        //获取第i个gesture
        String temp = "";
        while(char(this->infos.peek()) != '\n'){
          temp += char(this->infos.read());
        }
        this->gestures[i].equation = temp;
        this->gestures[i].device = this;
        this->infos.read();
      }
    }
    this->infos.close();
  }
}
