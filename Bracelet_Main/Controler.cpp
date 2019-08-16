#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Controler.h"

extern Device air_conditioner;
extern Device light;
extern Device television;
extern Device curtain;

Controler::Controler(){}

void Controler::setButtonA(bool state)
{
  buttonA = state;
}
void Controler::setButtonB(bool state)
{
  buttonB = state;
}
void Controler::setButtonC(bool state)
{
  buttonC = state;
}
void Controler::setButtonD(bool state)
{
  buttonD = state;
}
bool Controler::getButtonA()
{
  return buttonA;
}
bool Controler::getButtonB()
{
  return buttonB;
}
bool Controler::getButtonC()
{
  return buttonC;
}
bool Controler::getButtonD()
{
  return buttonD;
}
void Controler::initial()
{
  buttonA = false;
  buttonB = false;
  buttonC = false;
  buttonD = false;
}
Device* Controler::device()
{
  Device* device = NULL;
  if (buttonA)
    device = &air_conditioner;
  else if (buttonB)
    device = &light;
  else if (buttonC)
    device = &television;
  else if (buttonD)
    device = &curtain;
  return device;
}
void Controler::send(Order* order)
{
  if (order != NULL)
    order->getDevice()->getIrsend().sendRaw(order->getCode().buf, order->getCode().len, order->getCode().hz);
}
void Controler::terminate()
{
  buttonA = false;
  buttonB = false;
  buttonC = false;
  buttonD = false;
  //todo: C++垃圾回收机制
}
bool Controler::isPressing()
{
  Controler::detect();
  if (buttonA || buttonB || buttonC || buttonD)
    return true;
  else
    return false;
}
byte Controler::detect(){
  if (digitalRead(BUTTONA_PIN) == HIGH) {
    setButtonA(true);
    digitalWrite(LEDA_PIN,HIGH);
    return 1;
  }
  else{
    setButtonA(false);
    digitalWrite(LEDA_PIN,LOW);
  }
    
  if (digitalRead(BUTTONB_PIN) == HIGH) {
    setButtonB(true);
    return 2;
  }
  else
    setButtonB(false);
  if (digitalRead(BUTTONC_PIN == HIGH)) {
    setButtonC(true);
    return 3;
  }
  else
    setButtonC(false);
  if (digitalRead(BUTTOND_PIN == HIGH)) {
    setButtonD(true);
    return 4;
  }
  else
    setButtonD(false);
    return 0;
}
