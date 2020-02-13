#ifndef Device_H
#define Device_H
#if defined(ARDUINO)&&ARDUINO>=100
  #include "Arduino.h"
#else 
  #include "WProgram.h"
#endif

#include "Setting.h"
#include <SD.h>
#include "Gest_Quantity_Data.h"

class Gest_Data;
class Device{
  public:
    Device(String name, File infos,int orderNum);
    String getName();
    File getInfos();
    int getOrderNum();
    String* getOrderTypes();
    Code* getCodings();
    Gest_Data* getGestures();
    void complete();
    void three_clear();

  private:
    String name;
    File infos;
    int orderNum;
    //todo: 下面三个数组分分钟让内存爆掉
    String* orderTypes = NULL;
    Code* codings = NULL;
    Gest_Data* gestures = NULL;
};

#endif
