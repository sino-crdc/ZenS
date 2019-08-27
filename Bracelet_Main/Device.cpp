#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Device.h"

//todo 设置机制，将SD卡中存储数据，有用的或者频繁使用的部分加载到EEPROM
Device::Device(String name, File infos, int orderNum) {
  this->name = name;
  this->infos = infos;
  this->orderNum = orderNum;
  //Serial.println("device constructed.");
}

String Device::getName() {
  //  Serial.println("Device::getname");
  return this->name;
}

File Device::getInfos() {
  //  Serial.println("Device::getInfos");
  return this->infos;
}

int Device::getOrderNum() {
  //  Serial.println("Device::getOrderNum");
  return this->orderNum;
}

String* Device::getOrderTypes() {
  //  Serial.println("Device::getOrderTypes");
  if (this->orderTypes == NULL) {
    complete();
  }
  return this->orderTypes;
}

Code* Device::getCodings() {
  //  Serial.println("Device::getCodings");
  if (this->codings == NULL) {
    complete();
  }
  return this->codings;
}

Gest_Data* Device::getGestures() {
  //  Serial.println("Device::getGestures");
  if (this->gestures == NULL) {
    complete();
  }
  return this->gestures;
}

void Device::complete() {
  Serial.println("Device::complete");
  if (SD.begin(SD_PIN)) {
    if (this->infos = SD.open(this->infos.name(), FILE_READ)) {
      const int num = this->orderNum;
      this->orderTypes = new String[num];
      this->codings = new Code[num];
      this->gestures = new Gest_Data[num];
      for (int i = 0; this->infos.available() && i < num; i++) {

        //获取第i个orderType

        String data = "";

        while (this->infos.peek() != 0xFF) {
          data += char(this->infos.read());
        }
        this->orderTypes[i] = data;
        this->infos.read();//丢弃0xFF


        //获取第i个coding
        union xx {              //用于读取文件中的数据；
          unsigned int num = 0;
          byte buf[2];
        } code;
        unsigned int * int_coding_o = new unsigned int[243];
        int len = 0;
        for (int j = 0; j < 243; j++)
          int_coding_o[j] = 0;
        while (this->infos.available()) {
          code.buf[1] = this->infos.read();
          code.buf[0] = this->infos.peek();
          //          Serial.print((char)code.buf[1]);
          //          Serial.print(" ");
          //          Serial.print((char)code.buf[0]);
          //          Serial.print(" ");
          //          Serial.print(String(((code.buf[1]<<8)|code.buf[0])));
          //          Serial.print(" ");
          if (code.buf[0] != 0xFF && code.buf[1] != 0xFF)
            this->infos.read(), int_coding_o[len++] = ((code.buf[1] << 8) | code.buf[0]);
          else
            break;
          //          Serial.print(int_coding_o[len-1]),Serial.print(" ");
        }
        Serial.println();
        //            Serial.println(len);
        for (int j = 0; j < len; j++) {
          this->codings[i].buf[j] = int_coding_o[j];
          //              Serial.print(int_coding_o[i]),Serial.print(" ");
        }
        //            Serial.println();
        //int* temp_p = int_coding_o;
        //int* int_coding_o = (int*)by_coding_o;
        this->codings[i].len = len;
        this->codings[i].hz = 38;
        delete [] int_coding_o;
        if (this->infos.peek() == 0xFF)
          this->infos.read();

        //获取第i个gesture
        String temp = "";
        while (char(this->infos.peek()) != '\r') {
          temp += char(this->infos.read());
        }
        this->gestures[i].equation = temp;
        this->gestures[i].device = this;
        this->infos.read();//抛弃\r
        this->infos.read();//抛弃\n

      }
    }
    this->infos.close();
    Serial.println("Device::complete finish");
  }
}

void Device::three_clear() {
  delete [] this->orderTypes;
  delete [] this->codings;
  delete [] this->gestures;

  this->orderTypes = NULL;
  this->codings = NULL;
  this->gestures = NULL;
}
