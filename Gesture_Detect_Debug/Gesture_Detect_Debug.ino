#include <SoftwareSerial.h>

#define DAXTHRESHOLD 0.01//x轴加速度可标志临界变化量
#define DAYTHRESHOLD 0.01//y轴加速度可标志临界变化量
#define DAZTHRESHOLD 0.01//z轴加速度可标志临界变化量
#define ABSOLU_XA0 0.1//x轴绝对初始加速度
#define ABSOLU_YA0 0.1//y轴绝对初始加速度
#define ABSOLU_ZA0 1.1//z轴绝对初始加速度
#define ABSOLU_ANGLE0 0.0//初始绝对角度
#define WTHRESHOLD 50.0//角速度最大有效值
#define QUANTITY_AXE 'x'


#define GEST_RX_PIN 9//对应JY61的TX
#define GEST_TX_PIN 10//对应JY61的RX
#define BUTTON 13

static unsigned char Re_buf[11], counter = 0;
static unsigned char sign = 0;
static unsigned char wait = 50;
static bool first = true;//用于定性检测函数，是否是第一次传回加速度数据，用来判断以设置加速度初始值
static bool qfirst = true;
static float a[3], w[3], angle[3];

static SoftwareSerial sserial = SoftwareSerial(GEST_TX_PIN, GEST_RX_PIN);//9,10

String detect();
void serialEvent();
bool isPressing();
float quantity_detect();

void setup(){
  Serial.begin(9600);
  Serial.println("initializing...");
  sserial.begin(115200);
  Serial.println("baud rate：115200");
  sserial.write(0xFF);
  sserial.write(0xAA);
  sserial.write(0x61);
  Serial.println("set to serial communicaiton.");
  sserial.write(0xFF);
  sserial.write(0xAA);
  sserial.write(0x63);
  Serial.println("baud rate 115200, return rate 100HZ.");
  sserial.write(0xFF);
  sserial.write(0xAA);
  sserial.write(0x66);
  Serial.println("Vertical installation.");
  pinMode(BUTTON, INPUT);
}

void loop(){
  Serial.println("final equation: " + detect());
// quantity_detect();
}

bool isPressing(){
  return digitalRead(BUTTON) == HIGH;
}

String detect() {
  Serial.println("gesture detecting...");
  String equation = "";
  while (isPressing()) {
    //获取数据
    serialEvent();
    if (sign) { //若收到数据信号
      Serial.println("get sign.");
      sign = 0;
      //解析数据信号
      if (Re_buf[0] == 0x55 && Re_buf [1] == 0x51) { //检查帧头，识别到加速度包
        Serial.println("acceleration package gotten.");
        a[0] = (short(Re_buf [3] << 8 | Re_buf [2])) / 32768.0 * 16;//x
        a[1] = (short(Re_buf [5] << 8 | Re_buf [4])) / 32768.0 * 16;//y
        a[2] = (short(Re_buf [7] << 8 | Re_buf [6])) / 32768.0 * 16;//z
      } else {
        Serial.println("acceleration package not gotten");
        continue;
      }

      float xa0 = ABSOLU_XA0;
      float ya0 = ABSOLU_YA0;
      float za0 = ABSOLU_ZA0;

      if (first) { //设置加速度初始值
        Serial.println("acceleration initial set.");
        xa0 = a[0];
        ya0 = a[1];
        za0 = a[2];
        first = false;
      } else {
        //转换数据信号为手势表达式元素
        String tx = "";
        String ty = "";
        String tz = "";
        if (a[0] - xa0 > 0.01) {
          tx = "x+";
        }
        if (xa0 - a[0] > 0.01) {
          tx = "x-";
        }
        if (a[1] - ya0 > 0.01) {
          ty = "y+";
        }
        if (ya0 - a[1] > 0.01) {
          ty = "y-";
        }
        if (a[2] - za0 > 0.01) {
          tz = "z+";
        }
        if (za0 - a[2] > 0.01) {
          tz = "z-";
        }
        equation += tx + ty + tz;
        Serial.println(equation);
      }
    }
    delay(50+(wait++)%50);//判断循环条件->进入serialEvent->输出一个available，未执行完便直接开始新的循环(此函数)
  }

  first = true;

  return equation;
}

void quantity_detect() {
  Serial.println("quantity gesture detecting...");
  
  while(isPressing()){
      //获取数据
    serialEvent();
    if (sign) { //若收到数据信号
      Serial.println("get sign.");
      sign = 0;
      float angle0 = ABSOLU_ANGLE0;
      //解析数据信号
      if (Re_buf[0] == 0x55
          && Re_buf [1] == 0x53) { //检查帧头，识别到角度包
        Serial.println("Angle package gotten.");
        angle[QUANTITY_AXE - 120] = (short(Re_buf [(QUANTITY_AXE - 120) * 2 + 3] << 8 | Re_buf [(QUANTITY_AXE - 120) * 2 + 2])) / 32768.0 * 180;
        if (qfirst) { //设置角速度初始值
          Serial.println("set w initial.");
          angle0 = angle[QUANTITY_AXE - 120];
          qfirst = false;
        }
      } else if (Re_buf[0] == 0x55
                 && Re_buf [1] == 0x52) { //检查帧头，识别到角速度包
        Serial.println("w package gotten.");
        w[QUANTITY_AXE - 120] = (short(Re_buf [(QUANTITY_AXE - 120) * 2 + 3] << 8 | Re_buf [(QUANTITY_AXE - 120) * 2 + 2])) / 32768.0 * 2000;
        //如果角速度太快则挂起程序
        if (abs(w[QUANTITY_AXE - 120]) >= WTHRESHOLD) {
          Serial.println("w is too fast, hang the program.");
          qfirst = true;
          continue;
        }
      } else {
        continue;
      }

      if (!qfirst && abs(w[QUANTITY_AXE - 120]) < WTHRESHOLD) {
        //计算角度偏离量
        Serial.println("angle deviation calculating...");
        float deviation = angle[QUANTITY_AXE - 120] - angle0;
        deviation = deviation < -10.0 ? 180.0 + deviation : deviation;
        //发送角度
        Serial.print(deviation);
        Serial.print(" ");
      }
    }
  }
}

void serialEvent() {
  while (sserial.available()) {
    Serial.println("JY61 serial available.");
    //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code

    Re_buf[counter] = (unsigned char)sserial.read();
    if (counter == 0 && Re_buf[0] != 0x55) return; //第0号数据不是帧头
    counter++;
    if (counter == 11)          //接收到11个数据
    {
      counter = 0;             //重新赋值，准备下一帧数据的接收
      sign = 1;
      break;
    }
  }
  Serial.println("JY61 package reading end.");
}
