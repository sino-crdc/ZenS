#ifndef Setting_H
#define Setting_H
#if defined(ARDUINO)&&ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SD.h>

#define AC_ORDER_NUM 200
#define LIGHT_ORDER_NUM 50
#define TELE_ORDER_NUM 500
#define CURTAIN_ORDER_NUM 20

#define QUANTITY_AXE 'x'

#define SD_PIN 4
#define BUTTONA_PIN 10
#define BUTTONB_PIN 11
#define BUTTONC_PIN 12
#define BUTTOND_PIN 13
#define GEST_RX_PIN 8
#define GEST_TX_PIN 9

typedef struct{
  unsigned int buf[243];
  int len = 243;
  int hz = 38;
}Code;//todo: 要不要指针化？
#endif
