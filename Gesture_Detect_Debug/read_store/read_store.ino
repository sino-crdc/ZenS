/**
   read_store(即读即存)：
     每读取一包数据，先按照类别存到EEPROM或者SD卡中，之后在手势检测函数中生成手势表达式或者角度表达式
*/

#include <SoftwareSerial.h>
#include <SD.h>

#define DAXTHRESHOLD 0.5//x轴加速度可标志临界变化量
#define DAYTHRESHOLD 0.5//y轴加速度可标志临界变化量
#define DAZTHRESHOLD 0.5//z轴加速度可标志临界变化量
#define ABSOLU_XA0 1//x轴绝对初始加速度
#define ABSOLU_YA0 -1//y轴绝对初始加速度
#define ABSOLU_ZA0 1//z轴绝对初始加速度
#define ABSOLU_ANGLE0 0.0//初始绝对角度
#define WTHRESHOLD 50.0//角速度最大有效值
#define QUANTITY_AXE 'x'

#define SD_PIN 4

#define GEST_RX_PIN 9
#define GEST_TX_PIN 10
#define BUTTON 13

/*
   1. 配置MPU6050
   2. 等待按键
   3. 读取数据，传回数组
   4. 筛选数组
   5. 对应数组存入SD中
   6. 放开按键，完成读取
   7. 分析SD中数据生成表达式
*/

static unsigned char Re_buf[11], counter = 0;
static unsigned char sign = 0;
static bool first = true;//用于定性表达式生成函数，是否是第一个加速度数据，用来判断以设置加速度初始值
static bool qfirst = true;
static bool cfirst = true;//用于初次校准
static float a[3];

static SoftwareSerial sserial = SoftwareSerial(GEST_TX_PIN, GEST_RX_PIN);

File acceleration_signal;

String detect();
void serialEvent();
bool isPressing();
String equation_generate();
void simplify(String*);
void f_clear();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(4800);
  Serial.println("initializing...");

  sserial.begin(9600);
  Serial.println("baud rate：9600");

  byte baud[3] = {0xFF, 0xAA, 0x63};
  for (int i = 0; i < 3; i++) {
    sserial.write(baud[i]);
  }
  Serial.println("baud rate 115200, return rate 100Hz.");
  sserial.begin(115200);

  byte zzero[3] = {0xFF, 0xAA, 0x52};
  for (int i = 0; i < 3; i++) {
    sserial.write(zzero[i]);
  } for (int i = 0; i < 3; i++) {
    sserial.write(zzero[i]);
  }
  Serial.println("z-zeroing");

  byte acheck[3] = {0xFF, 0xAA, 0x67};
  for (int i = 0; i < 3; i++) {
    sserial.write(acheck[i]);
  } for (int i = 0; i < 3; i++) {
    sserial.write(acheck[i]);
  }
  Serial.println("A-calibration");

  pinMode(BUTTON, INPUT);
  pinMode(SD_PIN, OUTPUT);//  todo
  SD.begin(4);
}

void loop() {
  // put your main code here, to run repeatedly:

  acceleration_signal = SD.open("asignal.txt", FILE_WRITE);
  if (acceleration_signal) {
    if (f_clear()) {
      detect();
    } else {
      Serial.println("Error: file");
      return;
    }
  }
  acceleration_signal.close();
  acceleration_signal = SD.open("asignal.txt", FILE_READ);
  if (acceleration_signal && acceleration_signal.available()) {
    Serial.println(equation_generate());
  }
  acceleration_signal.close();
}

String equation_generate() {
  String equation = "";
  while (acceleration_signal.available()) {
    unsigned char signals[11] = {0};
    for (byte i = 0; i < 11; i++) {
      signals[i] = acceleration_signal.read();
    }

    if (signals[0] == 0x55 && signals [1] == 0x51) {
      a[0] = (short(signals [3] << 8 | signals [2])) / 32768.0 * 16;//x
      a[1] = (short(signals [5] << 8 | signals [4])) / 32768.0 * 16;//y
      a[2] = (short(signals [7] << 8 | signals [6])) / 32768.0 * 16;//z
    }

    float xa0 = ABSOLU_XA0;
    float ya0 = ABSOLU_YA0;
    float za0 = ABSOLU_ZA0;

    if (first) {
      //设置加速度初始值
      xa0 = a[0];
      ya0 = a[1];
      za0 = a[2];

      first = false;
    } else {
      //生成表达式项
      String tx = "";
      String ty = "";
      String tz = "";
      if (a[0] - xa0 > DAXTHRESHOLD) {
        //          Serial.print(a[0]-xa0);
        //          Serial.print(" ");
        tx = "x+";
      }
      if (xa0 - a[0] > DAXTHRESHOLD) {
        tx = "x-";
      }
      if (a[1] - ya0 > DAYTHRESHOLD) {
        //          Serial.print(a[1]-ya0);
        //          Serial.print(" ");
        ty = "y+";
      }
      if (ya0 - a[1] > DAYTHRESHOLD) {
        ty = "y-";
      }
      if (a[2] - za0 > DAZTHRESHOLD) {
        //          Serial.print(a[2]-za0);
        //          Serial.print(" ");
        tz = "z+";
      }
      if (za0 - a[2] > DAZTHRESHOLD) {
        tz = "z-";
      }
      equation += tx + ty + tz;
    }
  }
  return equation;
}

bool isPressing() {
  return digitalRead(BUTTON) == HIGH;
}

bool f_clear() {
  acceleration_signal.close();
  SD.remove(acceleration_signal);
  SD.mkdir("asignal.txt");
  acceleration_signal = SD.open("asignal.txt", FILE_WRITE);
  if (acceleration_signal) {
    return true;
  } else {
    return false;
  }
}

void detect() {
  while (isPressing()) {
    if (cfirst) {
      byte zzero[3] = {0xFF, 0xAA, 0x52};
      for (int i = 0; i < 3; i++) {
        sserial.write(zzero[i]);
      } for (int i = 0; i < 3; i++) {
        sserial.write(zzero[i]);
      }
      //    Serial.println("z-zeroing");

      byte acheck[3] = {0xFF, 0xAA, 0x67};
      for (int i = 0; i < 3; i++) {
        sserial.write(acheck[i]);
      } for (int i = 0; i < 3; i++) {
        sserial.write(acheck[i]);
      }
      //    Serial.println("A-calibration");

      cfirst = false;
    }
    //获取数据
    serialEvent();
    if (sign) { //若收到数据信号
      //      Serial.println("get sign.");
      sign = 0;
      //解析数据信号
      if (Re_buf[0] == 0x55 && Re_buf [1] == 0x51) { //检查帧头，识别到加速度包
        //        Serial.println("acceleration package gotten.");
        for (int i = 0; i < 11; i++) {
          acceleration_signal.write(Re_buf[i]);
        }
      } else {
        //        Serial.println("I love XiaoKe.");
        continue;
      }
    }
  }
}
cfirst = true;
}

void serialEvent() {
  while (sserial.available()) {
    //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code
    Re_buf[counter] = (unsigned char)sserial.read();
    if (counter == 0 && Re_buf[0] != 0x55) continue; //第0号数据不是帧头
    counter++;
    if (counter == 11)          //接收到11个数据
    {
      counter = 0;             //重新赋值，准备下一帧数据的接收
      sign = 1;
      break;
    }
  }
  //  Serial.println("JY61 package reading end.");//这个以及其他一系列的Serail调试信息输出占用很大一块儿时间，对帧传输的灵敏度可能产生影响
}

void simplify(String *s) {
  int len = s->length();
  int * f = (int *)malloc(6 * sizeof(int));
  char * c = (char *)malloc(len * sizeof(char));
  for (int i = 0; i < 6; i++)
    f[i] = 0;
  for (int i = 0; i < len; i++) {
    c[i] = s->charAt(i);
    if (i & 1) {
      int d = (c[i - 1] - 'x') * 2;
      if (c[i] == '-') d++;
      if (f[d])
        c[i - 1] = 0, c[i] = 0;
      else
        f[d] = 1;
    }
  }
  (*s) = "";
  for (int i = 0; i < len; i++) {
    if (c[i])
      s->concat(c[i]);
  }
  free(f);
  free(c);
}
