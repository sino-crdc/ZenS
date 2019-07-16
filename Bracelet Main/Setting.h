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

#define SD_PIN 53

#define RAY_PIN 3

#define BUTTONA_PIN 29
#define BUTTONB_PIN 30
#define BUTTONC_PIN 31
#define BUTTOND_PIN 32

#define GEST_RX_PIN 9
#define GEST_TX_PIN 10


#define LEDA_PIN 46
#define LEDB_PIN 47
#define LEDC_PIN 48
#define LEDD_PIN 49 

typedef struct{
  unsigned int buf[243];
  int len = 243;
  int hz = 38;
}Code;//todo: 要不要指针化？
#endif
