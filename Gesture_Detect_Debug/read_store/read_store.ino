/**
 * read_store(即读即存)：
 *   每读取一包数据，先按照类别存到EEPROM或者SD卡中，之后在手势检测函数中生成手势表达式或者角度表达式
 */

#include <SoftwareSerial.h>

#define DAXTHRESHOLD 0.5//x轴加速度可标志临界变化量
#define DAYTHRESHOLD 0.5//y轴加速度可标志临界变化量
#define DAZTHRESHOLD 0.5//z轴加速度可标志临界变化量
#define ABSOLU_XA0 1//x轴绝对初始加速度
#define ABSOLU_YA0 -1//y轴绝对初始加速度
#define ABSOLU_ZA0 1//z轴绝对初始加速度
#define ABSOLU_ANGLE0 0.0//初始绝对角度
#define WTHRESHOLD 50.0//角速度最大有效值
#define QUANTITY_AXE 'x'


#define GEST_RX_PIN 9
#define GEST_TX_PIN 10
#define BUTTON 13

 
/*
 * 1. 配置MPU6050
 * 2. 等待按键
 * 3. 读取数据，传回数组
 * 4. 筛选数组
 * 5. 对应数组存入EEPROM中
 * 6. 放开按键，完成读取
 * 7. 分析EEPROM中数据生成表达式
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(4800);
  Serial.println("initializing...");
  
  sserial.begin(9600);
  Serial.println("baud rate：115200");

  byte baud[3] = {0xFF,0xAA,0x63};
  for(int i = 0; i<3; i++){
    sserial.write(baud[i]);
  }
  Serial.println("baud rate 115200, return rate 100Hz.");
  sserial.begin(115200);

  byte zzero[3]={0xFF,0xAA,0x52};
  for(int i = 0; i<3; i++){
    sserial.write(zzero[i]);
  }for(int i = 0; i<3; i++){
    sserial.write(zzero[i]);
  }
  Serial.println("z-zeroing");
    
  byte acheck[3]={0xFF,0xAA,0x67};
  for(int i = 0; i<3; i++){
    sserial.write(acheck[i]);
  }for(int i = 0; i<3; i++){
    sserial.write(acheck[i]);
  }
  Serial.println("A-calibration");

  pinMode(BUTTON, INPUT);
}

String detect();
void serialEvent();
bool isPressing();
String equation_generate();
void quantity_detect();
void simplify(String*);

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("final equation: " + detect());

}
