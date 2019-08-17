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

Controler::Controler() {Serial.println("controler constructed.");}

void Controler::setButtonA(bool state)
{
  buttonA = state;
  Serial.println("buttonA == " + buttonA);
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
  Serial.println("controler initializing...");
  buttonA = false;
  buttonB = false;
  buttonC = false;
  buttonD = false;
  Serial.println("controler initialized.");
}
Device* Controler::device()
{
  Serial.println("device searching...");
  Device* device = NULL;
  Serial.println("device == NULL");
  if (buttonA){
    device = &air_conditioner;
    Serial.println("device == air_conditioner");
  }
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
  Serial.println("order sending...");
  if (order != NULL) {
    Serial.print("order sent: ");
    Serial.println("device^"+order->getDevice()->getName() + " ordertype^"+order->getOrderType());
    order->getDevice()->getIrsend().sendRaw(order->getCode().buf, order->getCode().len, order->getCode().hz);
  } else {
    Serial.println("order == NULL");
  }
}
void Controler::terminate()
{
  Serial.println("controler terminating...");
  buttonA = false;
  buttonB = false;
  buttonC = false;
  buttonD = false;
  //todo: C++垃圾回收机制
  Serial.println("controler terminated.");
}
bool Controler::isPressing()
{
  Controler::detect();
  if (buttonA || buttonB || buttonC || buttonD) {
    Serial.println("pressing.");
    return true;
  }
  else {
    Serial.println("not pressing.");
    return false;
  }
}
byte Controler::detect() {
  Serial.println("detecting...");
  if (digitalRead(BUTTONA_PIN) == HIGH) {
    //Serial.println("A");
    setButtonA(true);
    digitalWrite(LEDA_PIN, HIGH);
    Serial.println("ledA == HIGH");
    return 1;
  }
  else {
    setButtonA(false);
    digitalWrite(LEDA_PIN, LOW);
  }

  if (digitalRead(BUTTONB_PIN) == HIGH) {
    //Serial.println("B");
    setButtonB(true);
    return 2;
  }
  else
    setButtonB(false);
  if (digitalRead(BUTTONC_PIN) == HIGH) {
    //Serial.println("C");
    setButtonC(true);
    return 3;
  }
  else
    setButtonC(false);
    
  if (digitalRead(BUTTOND_PIN) == HIGH) {
    //Serial.println("D");
    setButtonD(true);
    return 4;
  }
  else
    setButtonD(false);
  return 0;
}