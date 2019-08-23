#include <SoftwareSerial.h>

#define DAXTHRESHOLD 0.5//x轴加速度可标志临界变化量
#define DAYTHRESHOLD 0.5//y轴加速度可标志临界变化量
#define DAZTHRESHOLD 0.5//z轴加速度可标志临界变化量
#define ABSOLU_XA0 0.1//x轴绝对初始加速度
#define ABSOLU_YA0 0.1//y轴绝对初始加速度
#define ABSOLU_ZA0 1.1//z轴绝对初始加速度
#define ABSOLU_ANGLE0 0.0//初始绝对角度
#define WTHRESHOLD 50.0//角速度最大有效值
#define QUANTITY_AXE 'x'


#define GEST_RX_PIN 9
#define GEST_TX_PIN 10
#define BUTTON 13

static unsigned char Re_buf[11], counter = 0;
static unsigned char sign = 0;
static unsigned char wait = 50;
static bool first = true;//用于定性检测函数，是否是第一次传回加速度数据，用来判断以设置加速度初始值
static bool qfirst = true;
static float a[3], w[3], angle[3];


String detect();
void serialEvent();
bool isPressing();
void quantity_detect();
void simplify(String*);

void setup(){
  Serial.begin(115200);
  Serial.println("initializing...");
  
  Serial1.begin(115200);
  Serial.println("baud rate：115200");
  
  char vertical[3] = {0xFF,0xAA,0x66};
  for(int i=0;i<3;i++)
    Serial1.write(vertical[i]);
  Serial.println("Vertical installation.");
  
  char communication[3]={0xFF,0xAA,0x61};
  for(int i=0;i<3;i++)
    Serial1.write(communication[i]);
  Serial.println("set to serial communicaiton.");
  
  char baud[3] = {0xFF,0xAA,0x63};
  for(int i=0;i<3;i++)
    Serial1.write(baud[i]);
  Serial.println("baud rate 115200, return rate 100Hz.");
  
  pinMode(BUTTON, INPUT);
}

void loop(){
  Serial.println("final equation: " + detect());

// quantity_detect();
 
//  while(isPressing()){
//    serialEvent();
//    Serial.println();
//  }
}

bool isPressing(){
  delay(100);
  return 1;//digitalRead(BUTTON) == HIGH;
}

String detect() {
  Serial.println("gesture detecting...");
  String equation = "";
  while (isPressing()) {
    //delay(500);
    //获取数据
    serialEvent();
    if (sign) { //若收到数据信号
      //Serial.println("get sign.");
      sign = 0;
      //解析数据信号
      if (Re_buf[0] == 0x55 && Re_buf [1] == 0x51) { //检查帧头，识别到加速度包
        //Serial.println("acceleration package gotten.");
        a[0] = (short(Re_buf [3] << 8 | Re_buf [2])) / 32768.0 * 16;//x
        a[1] = (short(Re_buf [5] << 8 | Re_buf [4])) / 32768.0 * 16;//y
        a[2] = (short(Re_buf [7] << 8 | Re_buf [6])) / 32768.0 * 16;//z
      } else if(Re_buf[0] == 0x55 && Re_buf [1] == 0x52){
        //Serial.println("angle package gotten");
        continue;
      } else if(Re_buf[0] == 0x55 && Re_buf [1] == 0x53){
        //Serial.println("w package gotten");
        continue;
      }else{
        //Serial.println("I love XiaoKe.");
        continue;
      }

      float xa0 = ABSOLU_XA0;
      float ya0 = ABSOLU_YA0;
      float za0 = ABSOLU_ZA0;
      for(int i=0;i<3;i++)
        Serial.print(String(a[i])+" ");
      Serial.println();
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
        if (a[0] - xa0 > DAXTHRESHOLD) {
          tx = "x+";
        }
        if (xa0 - a[0] > DAXTHRESHOLD) {
          tx = "x-";
        }
        if (a[1] - ya0 > DAYTHRESHOLD) {
          ty = "y+";
        }
        if (ya0 - a[1] > DAYTHRESHOLD) {
          ty = "y-";
        }
        if (a[2] - za0 > DAZTHRESHOLD) {
          tz = "z+";
        }
        if (za0 - a[2] > DAZTHRESHOLD) {
          tz = "z-";
        }
        equation = tx + ty + tz;
        simplify(&equation);
        Serial.println(equation);
      }
    }
    char zzero[3]={0xFF,0xAA,0x52};
    for(int i=0;i<3;i++)
      Serial1.write(zzero[i]);
    
    char acheck[3]={0xFF,0xAA,0x67};
    for(int i=0;i<3;i++)
      Serial1.write(acheck[i]);
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
          Serial.println("set a initial.");
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
    char zzero[3]={0xFF,0xAA,0x52};
    Serial1.write(zzero,3);
  }
}

void serialEvent() {
  int sum=0;
  while (Serial1.available()) { 
    //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code
    //Serial.println("available.");
    Re_buf[counter] = (unsigned char)Serial1.read();
    if (counter == 0 && Re_buf[0] != 0x55) continue;//第0号数据不是帧头
    sum+=Re_buf[counter];
    counter++;
    if (counter == 11)          //接收到11个数据
    {
      //sum-=Re_buf[counter-1];
      //sum&=0xFF;
      //if(sum==Re_buf[counter-1]){
        counter = 0;             //重新赋值，准备下一帧数据的接收
        sign = 1;
      //}
      break;
    }
  }
  //Serial.println("JY61 package reading end.");//这个以及其他一系列的Serail调试信息输出占用很大一块儿时间，对帧传输的灵敏度产生较大影响
}

void simplify(String *s){
  int len=s->length();
  int * f=(int *)malloc(6*sizeof(int));
  char * c=(char *)malloc(len*sizeof(char));
  for(int i=0;i<6;i++) 
    f[i]=0;
  for(int i=0;i<len;i++){
    c[i]=s->charAt(i);
    if(i&1){
        int d=(c[i-1]-'x')*2;
        if(c[i]=='-') d++;
        if(f[d])
          c[i-1]=0,c[i]=0;
        else
          f[d]=1;
      }
  }
  (*s)="";
  for(int i=0;i<len;i++){
      if(c[i])
        s->concat(c[i]);
    }
  free(f);
  free(c);
}
