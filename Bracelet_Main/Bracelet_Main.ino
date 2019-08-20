#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Setting.h"
#include <IRremote.h>
#include <SD.h>
#include "Device.h"
#include "Gesture.h"
#include "Order.h"
#include "Controler.h"

Controler controler;

Device air_conditioner = Device("AC", File(SdFile(), "AC.txt"), AC_ORDER_NUM);
Device light = Device("Light", File(SdFile(), "Light.txt"), LIGHT_ORDER_NUM);
Device television = Device("Tele", File(SdFile(), "Tele.txt"), TELE_ORDER_NUM);
Device curtain = Device("Curtain", File(SdFile(), "Curtain.txt"), CURTAIN_ORDER_NUM);

//todo: 关于struct的指针化，String的问题(string、String.h)
void setup() {
  Serial.begin(115200);
  controler.initial();
  Gesture::initial();
  pinMode(BUTTONA_PIN, INPUT);
  pinMode(BUTTONB_PIN, INPUT);
  pinMode(BUTTONC_PIN, INPUT);
  pinMode(BUTTOND_PIN, INPUT);
  pinMode(LEDA_PIN, OUTPUT);
  pinMode(LEDB_PIN, OUTPUT);
  pinMode(LEDC_PIN, OUTPUT);
  pinMode(LEDD_PIN, OUTPUT);
  pinMode(SD_PIN, OUTPUT);
}

void loop() {
  Serial.println("loop begin");
  byte pin0 = controler.detect();//第一次按下的按键
  if (controler.isPressing()) {
    Serial.println("in the work body.");
    Device* device = controler.device();
    if (device == NULL) {
      Serial.println("device == NULL");
      return;
    }
    if(device != NULL){
      Serial.println("device gotten.");
    }
    Gesture gesture = Gesture(device);
    Order* order = gesture.analyze(gesture.detect());
    if (order != NULL) {
      Serial.println("order is not NULL");
      if (!order->getIsQuantity()) {
        controler.send(order);
        Serial.println("order not isQuantity finish");
      }else{
        unsigned long time0 = millis();//开始计时
        while (millis() - time0 < 10000L) { //小于十秒

          bool fly = false;//用于后续嵌套循环的双跳

          byte pin1 = controler.detect();
          Serial.println("pin1 detect: "+pin1);
          if(controler.isPressing() && pin1 == pin0){//如果按键按下并且是相同设备
            Device* device_now = controler.device();
            Gesture gesture0 = Gesture(device_now);
            do {
              controler.send(gesture0.quantity_analyze(gesture0.quantity_detect(order)));
              byte pin1 = controler.detect();
              Serial.println("pin1 detect: "+pin1);
              if(!(controler.isPressing() && pin1 == pin0)){
                fly = true;//“要跳出去啦啦啦~”
                break;//跳出内层
              }
            } while ( controler.isPressing() && pin1 == pin0); //如果按键按下并且是相同设备
            //Device派生类成员变量的动态存储撤销
            delete [] device_now->getOrderTypes();
            delete [] device_now->getCodings();
            delete [] device_now->getGestures();
          }
          if (fly) { //“啊，我终于要跳出去啦啦啦~”
            break;//跳出外层
          }
        }
        Serial.println("order isQuantity finish");
      }
    }
    controler.terminate();
  }
}
