#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Controler.h"

/*
 * 这些变量无法在类体中定义，只能在类体中声明，类外定义
 * 但是这样的话会出现multiple definition的问题
 * 因为编译器会将Controler相关的.cpp文件和.h文件中所有的类外赋值(包括.cpp中定义的函数)当作定义
 * 这是编译器的固有问题
 * 因此一个解决方法是，不在类体中声明，这样编译器就不会要求“类外定义”，因为本来就在类外
 * 于是，类外声明的静态变量，可以在类外初始化定义，也可以在类外定义的函数中去赋值
 * 不过，仍然在类体中用注释标记出这些变量以增强可读性
 */
static bool buttonA = false;
static bool buttonB = false;
static bool buttonC = false;
static bool buttonD = false;

void Controler::setButtonA(bool state)
{
  buttonA = state;
  byte stay = state ? 100 : 50;
  digitalWrite(LEDA_PIN,HIGH);
  delay(stay);
  digitalWrite(LEDA_PIN,LOW);
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
  detect();
  if (buttonA || buttonB || buttonC || buttonD)
    return true;
  else
    return false;
}
byte Controler::detect(){
  if (digitalRead(BUTTONA_PIN) == HIGH) {
    setButtonA(true);
    return 1;
  }
  else
    setButtonA(false);
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
