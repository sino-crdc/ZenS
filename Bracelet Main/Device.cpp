#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Device.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])
#endif

unsigned char by_coding[243];
unsigned int int_coding[972];

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
      //todo: sizeof(String) sizeof(Gest_Data) sizeof(Device) 可求吗？
      this->orderTypes = (String*)malloc(sizeof(String)*this->orderNum);
      this->codings = (Code*)malloc(sizeof(Code)*this->orderNum);
      this->gestures = (Gest_Data*)malloc(sizeof(Gest_Data)*this->orderNum);
      for(int i = 0;this->infos.available() && i<orderNum;i++){
        
        //获取第i个orderType
        String data = "";
        while(char(this->infos.peek()) != '#'){
          data += char(this->infos.read());
        }
        this->orderTypes[i] = data;
        this->infos.read();//丢弃'#'

        //获取第i个coding
        for(int j = 0;char(this->infos.peek()) != '#';j++){
          by_coding[j] = char(this->infos.read());
        }

        byteTointArray(by_coding,int_coding);
        
        for(int j = 0;j < 243; j++){
          this->codings[i].buf[j] = int_coding[j];  
        }
        
        this->codings[i].len = ARRAY_SIZE(int_coding);
        this->codings[i].hz = 38;
        this->infos.read();
         
        //获取第i个gesture
        String temp = "";
        while(char(this->infos.peek()) != '\n'){
          temp += char(this->infos.read());
        }
        this->gestures[i].equation = temp;
        this->gestures[i].device = this;
        this->infos.read();

        //malloc的空间不free
      }
    }
    this->infos.close();
  }
}
void Device::byteTointArray(char* bytes,int* ints){
  for(int i = 3; i<sizeof(int)*243; i+=4){
     ints[(i+1)/4-1] = int(bytes[i-3]<<12 | bytes[i-2]<<8 | bytes[i-1]<<4 | bytes[i]);
  }
}
