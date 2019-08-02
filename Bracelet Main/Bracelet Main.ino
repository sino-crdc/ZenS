#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <IRremote.h>
#include <SD.h>
#include "Controler.h"
#include "Device.h"
#include "Gesture.h"
#include "Order.h"
#include "Setting.h"


//todo: 关于struct的指针化，String的问题(string、String.h)
void setup() {
  Controler::initial();
  Gesture::initial();
  pinMode(BUTTONA_PIN, INPUT);
  pinMode(BUTTONB_PIN, INPUT);
  pinMode(BUTTONC_PIN, INPUT);
  pinMode(BUTTOND_PIN, INPUT);
}

void loop() {
  byte pin0 = Controler::detect();//第一次按下的按键
  if (Controler::isPressing()) {
    Device* device = Controler::device();
    if(device == NULL){
      return;
    }
    Gesture gesture = Gesture(device);
    Order* order = gesture.analyze(gesture.detect());
    if (order != NULL) {
      if (!order->getIsQuantity()) {
        Controler::send(order);
      }else{
        unsigned long time0 = millis();//开始计时
        while(millis() - time0 < 10000L){//小于十秒

          bool fly = false;//用于后续嵌套循环的双跳

          byte pin1 = Controler::detect();
          if(Controler::isPressing() && pin1 == pin0){//如果按键按下并且是相同设备
            Gesture gesture0 = Gesture(Controler::device());
            do{
              Controler::send(gesture0.quantity_analyze(gesture0.quantity_detect(order)));
              byte pin1 = Controler::detect();
              if(!(Controler::isPressing() && pin1 == pin0)){
                fly = true;//“要跳出去啦啦啦~”
                break;//跳出内层
              }
            }while( Controler::isPressing() && pin1 == pin0);//如果按键按下并且是相同设备
          }
          if(fly){//“啊，我终于要跳出去啦啦啦~”
            break;//跳出外层
          }
        }
      }
    }
    Controler::terminate();
  }
}