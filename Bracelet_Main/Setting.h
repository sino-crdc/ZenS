#ifndef Setting_H
#define Setting_H
#if defined(ARDUINO)&&ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SD.h>

#define AC_ORDER_NUM 50
#define LIGHT_ORDER_NUM 10
#define TELE_ORDER_NUM 200 //这个占用了300KB的动态内存，可以过更改数量为50或者40，如果存在ROM中，减少到250以内(250爆满)
#define CURTAIN_ORDER_NUM 5

#define QUANTITY_AXE 'x'

#define SD_PIN 4

#define RAY_PIN 7

#define BUTTONA_PIN 25
#define BUTTONB_PIN 36
#define BUTTONC_PIN 27
#define BUTTOND_PIN 28

#define GEST_RX_PIN 18
#define GEST_TX_PIN 19


#define LEDA_PIN 47
#define LEDB_PIN 48
#define LEDC_PIN 49
#define LEDD_PIN 50

typedef struct{
  unsigned int buf[243]={0};
  int len = 243;
  int hz = 38;
}Code;//todo: 要不要指针化？
#endif
