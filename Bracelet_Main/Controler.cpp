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
IRsend irsend;

Controler::Controler(){}

void Controler::setButtonA(bool state)
{
  buttonA = state;
//  Serial.println("buttonA == " + String(buttonA));
}
void Controler::setButtonB(bool state)
{
  buttonB = state;
//  Serial.println("buttonB == " + String(buttonB));
}
void Controler::setButtonC(bool state)
{
  buttonC = state;
//  Serial.println("buttonC == " + String(buttonC));
}
void Controler::setButtonD(bool state)
{
  buttonD = state;
//  Serial.println("buttonD == " + String(buttonD));
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
    device = &light;
    Serial.println("device == light");
  }
  else if (buttonB)
    device = &air_conditioner;
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
    irsend.sendRaw(order->getCode().buf, order->getCode().len, order->getCode().hz);//可能需要加上循环连环发射密集轰炸
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
//    Serial.println("pressing.");
    return true;
  }
  else {
//    Serial.println("not pressing.");
    return false;
  }
}
byte Controler::detect() {
//  Serial.println("detecting...");
  if (digitalRead(BUTTONA_PIN) == HIGH) {
    setButtonA(true);
    digitalWrite(LEDA_PIN, HIGH);
//    Serial.println("ledA == HIGH");
    return 1;
  }
  else {
    setButtonA(false);
    digitalWrite(LEDA_PIN, LOW);
  }

  if (digitalRead(BUTTONB_PIN) == HIGH) {
    setButtonB(true);
    digitalWrite(LEDB_PIN, HIGH);
//    Serial.println("ledB == HIGH");
    return 2;
  }
  else{
    setButtonB(false);
    digitalWrite(LEDB_PIN, LOW);
  }
    
  if (digitalRead(BUTTONC_PIN) == HIGH) {
    setButtonC(true);
    digitalWrite(LEDC_PIN, HIGH);
//    Serial.println("ledC == HIGH");
    return 3;
  }
  else{
    setButtonC(false);
    digitalWrite(LEDC_PIN, LOW);
  }
    
    
  if (digitalRead(BUTTOND_PIN) == HIGH) {
    setButtonD(true);
    digitalWrite(LEDD_PIN, HIGH);
//    Serial.println("ledD == HIGH");
    return 4;
  }
  else{
    setButtonD(false);
    digitalWrite(LEDD_PIN, LOW);
  }
    
  return 0;
}
