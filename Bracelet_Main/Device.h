#ifndef Device_H
#define Device_H
#if defined(ARDUINO)&&ARDUINO>=100
  #include "Arduino.h"
#else 
  #include "WProgram.h"
#endif

#include "Setting.h"
#include <IRremote.h>
#include <SD.h>
#include "Gest_Quantity_Data.h"

class Gest_Data;
class IRsend;

class Device{
  public:
    Device(String name, IRsend irsend, File infos,int orderNum);
    String getName();
    IRsend getIrsend();
    File getInfos();
    int getOrderNum();
    String* getOrderTypes();
    Code* getCodings();
    Gest_Data* getGestures();//todo:指针还是指针数组
    void complete();

  private:
    String name;
    IRsend irsend;
    File infos;
    int orderNum;
    String* orderTypes = NULL;
    Code* codings = NULL;
    Gest_Data* gestures = NULL;

    void byteTointArray(char* bytes,int* ints);
    
};

//todo: 这些我想放在Setting.h里面
Device air_conditioner = Device("Air_Conditioner", IRsend(), File(SdFile(),"Air_Conditioner.infos"), AC_ORDER_NUM);
Device light = Device("Light", IRsend(), File(SdFile(),"Light.infos"),LIGHT_ORDER_NUM);
Device television = Device("Television", IRsend(), File(SdFile(),"Television.infos"), TELE_ORDER_NUM);
Device curtain = Device("Curtain", IRsend(), File(SdFile(),"Curtain.infos"), CURTAIN_ORDER_NUM);
#endif
