/*
   read_process(即读即理):
      每读取一包数据立马传到手势检测函数中生成手势表达式或者角度表达式
*/
#define DAXTHRESHOLD 1.0//x轴加速度可标志临界变化量
#define DAYTHRESHOLD 1.0//y轴加速度可标志临界变化量
#define DAZTHRESHOLD 1.0//z轴加速度可标志临界变化量
#define ABSOLU_XA0 0//x轴绝对初始加速度
#define ABSOLU_YA0 0//y轴绝对初始加速度
#define ABSOLU_ZA0 1//z轴绝对初始加速度
#define ABSOLU_ANGLE0 0.0//初始绝对角度
#define WTHRESHOLD 500.0//角速度最大有效值
#define QUANTITY_AXE 'x'

#define BUTTON 25

static unsigned char Re_buf[11], counter = 0;
static unsigned char sign = 0;
static bool first = true;//用于定性检测函数，是否是第一次传回加速度数据，用来判断以设置加速度初始值
static bool qfirst = true;
static bool cfirst = true;//用于初次校准
static float a[3], w[3], angle[3];

String detect();
void serialEvent();
bool isPressing();
void quantity_detect();
void simplify(String*);

void setup() {
  Serial.begin(4800);
  Serial.println("initializing...");

  Serial1.begin(9600);
  Serial.println("baud rate：115200");

  byte baud[3] = {0xFF, 0xAA, 0x63};
  for (int i = 0; i < 3; i++) {
    Serial1.write(baud[i]);
  }
  Serial.println("baud rate 115200, return rate 100Hz.");
  Serial1.begin(115200);

  byte zzero[3] = {0xFF, 0xAA, 0x52};
  for (int i = 0; i < 3; i++) {
    Serial1.write(zzero[i]);
  } for (int i = 0; i < 3; i++) {
    Serial1.write(zzero[i]);
  }
  Serial.println("z-zeroing");

  byte acheck[3] = {0xFF, 0xAA, 0x67};
  for (int i = 0; i < 3; i++) {
    Serial1.write(acheck[i]);
  } for (int i = 0; i < 3; i++) {
    Serial1.write(acheck[i]);
  }
  Serial.println("A-calibration");

  pinMode(BUTTON, INPUT);
}

void loop() {
  //Serial.println("final equation: " + detect());

  detect();

  //  while(isPressing()){
  //    serialEvent();
  //    Serial.println();
  //  }
}

bool isPressing() {
  return digitalRead(BUTTON) == HIGH;
}

String detect() {
  Serial.println("gesture detecting...");
  String equation = "";
  float xa0 = ABSOLU_XA0;
  float ya0 = ABSOLU_YA0;
  float za0 = ABSOLU_ZA0;
  while (isPressing()) {
    if (cfirst) {
      byte zzero[3] = {0xFF, 0xAA, 0x52};
      for (int i = 0; i < 3; i++) {
        Serial1.write(zzero[i]);
      } for (int i = 0; i < 3; i++) {
        Serial1.write(zzero[i]);
      }
      //    Serial.println("z-zeroing");

      byte acheck[3] = {0xFF, 0xAA, 0x67};
      for (int i = 0; i < 3; i++) {
        Serial1.write(acheck[i]);
      } for (int i = 0; i < 3; i++) {
        Serial1.write(acheck[i]);
      }
      //    Serial.println("A-calibration");

      cfirst = false;
      //delay(2000);
    }
    //获取数据
    serialEvent();
    if (sign) { //若收到数据信号
      //      Serial.println("get sign.");
      sign = 0;
      //解析数据信号
      if (Re_buf[0] == 0x55 && Re_buf [1] == 0x51) { //检查帧头，识别到加速度包
        //        Serial.println("acceleration package gotten.");
        a[0] = (short(Re_buf [3] << 8 | Re_buf [2])) / 32768.0 * 16;//x
        a[1] = (short(Re_buf [5] << 8 | Re_buf [4])) / 32768.0 * 16;//y
        a[2] = (short(Re_buf [7] << 8 | Re_buf [6])) / 32768.0 * 16;//z
      } else if (Re_buf[0] == 0x55 && Re_buf [1] == 0x52) {
        //        Serial.println("angle package gotten");
        continue;
      } else if (Re_buf[0] == 0x55 && Re_buf [1] == 0x53) {
        //        Serial.println("w package gotten");
        continue;
      } else {
        //        Serial.println("I love XiaoKe.");
        continue;
      }

      if (first) { //设置加速度初始值
        //        Serial.println("acceleration initial set.");
        xa0 = a[0];
        //        Serial.print(xa0);
        //        Serial.print(" ");
        ya0 = a[1];
        //        Serial.print(ya0);
        //        Serial.print(" ");
        za0 = a[2];
        //        Serial.print(za0);
        //        Serial.println(" ");
        first = false;
        //Serial.println("xa0: " + String(xa0) + " ya0: " + String(ya0) + " za0: " + String(za0));
        delay(4);
      } else {
        //转换数据信号为手势表达式元素
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
        simplify(&equation);
        //Serial.println(equation);
        Serial.println("xa-xa0: " + String(a[0] - xa0) + " ya-ya0: " + String(a[1] - ya0) + " za-za0: " + String(a[2] - za0));
      }
    }
  }

  first = true;
  cfirst = true;

  return equation;
}

void quantity_detect() {
  Serial.println("quantity gesture detecting...");
  float angle0 = ABSOLU_ANGLE0;
  while (isPressing()) {
    //获取数据
    if (cfirst) {
      byte zzero[3] = {0xFF, 0xAA, 0x52};
      for (int i = 0; i < 3; i++) {
        Serial1.write(zzero[i]);
      } for (int i = 0; i < 3; i++) {
        Serial1.write(zzero[i]);
      }
      //    Serial.println("z-zeroing");

      byte acheck[3] = {0xFF, 0xAA, 0x67};
      for (int i = 0; i < 3; i++) {
        Serial1.write(acheck[i]);
      } for (int i = 0; i < 3; i++) {
        Serial1.write(acheck[i]);
      }
      //    Serial.println("A-calibration");

      cfirst = false;
    }
    serialEvent();
    if (sign) { //若收到数据信号
      //      Serial.println("get sign.");
      sign = 0;
      //解析数据信号
      if (Re_buf[0] == 0x55
          && Re_buf [1] == 0x53) { //检查帧头，识别到角度包
        //        Serial.println("Angle package gotten.");
        angle[QUANTITY_AXE - 120] = (short(Re_buf [(QUANTITY_AXE - 120) * 2 + 3] << 8 | Re_buf [(QUANTITY_AXE - 120) * 2 + 2])) / 32768.0 * 180;
        if (qfirst) { //设置角度初始值
          //          Serial.print("set angle initial:");
          angle0 = angle[QUANTITY_AXE - 120];
          //          Serial.println(angle0);
          qfirst = false;
        }
      }
      //      else if (Re_buf[0] == 0x55
      //                 && Re_buf [1] == 0x52) { //检查帧头，识别到角速度包
      //        Serial.println("w package gotten.");
      //        w[QUANTITY_AXE - 120] = (short(Re_buf [(QUANTITY_AXE - 120) * 2 + 3] << 8 | Re_buf [(QUANTITY_AXE - 120) * 2 + 2])) / 32768.0 * 2000;
      //        //如果角速度太快则挂起程序
      //        if (abs(w[QUANTITY_AXE - 120]) >= WTHRESHOLD) {
      //          Serial.println("w is too fast, hang the program.");
      //          qfirst = true;
      //          continue;
      //        }
      //      }
      else {
        continue;
      }

      //      if (!qfirst && abs(w[QUANTITY_AXE - 120]) < WTHRESHOLD) {
      if (!qfirst) {
        //计算角度偏离量
        //        Serial.println("angle deviation calculating...");
        float deviation = angle[QUANTITY_AXE - 120] - angle0;
        //        deviation = deviation < -10.0 ? 180.0 + deviation : deviation;
        //发送角度
        //        if(deviation<=100.0)
        Serial.println(deviation);
        //Serial.print(" ");
        //      }
      }
    }
  }
  cfirst = true;
  qfirst = true;
}

void serialEvent() {
  unsigned char sum = 0;
  while (Serial1.available()) {
    //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code
    Re_buf[counter] = (unsigned char)Serial1.read();
    if (counter == 0 && Re_buf[0] != 0x55) continue;//第0号数据不是帧头
    if (counter < 10)
      sum += Re_buf[counter];
    counter++;
    if (counter == 11)          //接收到11个数据
    {
      //      sum &= 0xFF;
      //      Serial.print(sum,DEC);
      //      Serial.print(" ");
      //      Serial.println(Re_buf[counter-1]);
      delay(4);
      if (sum == Re_buf[counter - 1]) {
        //重新赋值，准备下一帧数据的接收
        sign = 1;
      }
      counter = 0;
      break;
    }
    //    Serial.println(sign);
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
