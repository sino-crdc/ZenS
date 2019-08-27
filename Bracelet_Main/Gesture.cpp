#if ARDUINO >= 100
#include "Arduino.h"
#else

#include "WProgram.h"

#endif

#include "Gesture.h"

#define DAXTHRESHOLD 2//x轴加速度可标志临界变化量
#define DAYTHRESHOLD 2//y轴加速度可标志临界变化量
#define DAZTHRESHOLD 2//z轴加速度可标志临界变化量
#define ABSOLU_XA0 1//x轴绝对初始加速度
#define ABSOLU_YA0 -1//y轴绝对初始加速度
#define ABSOLU_ZA0 1//z轴绝对初始加速度
#define ABSOLU_ANGLE0 0.0//初始绝对角度
#define WTHRESHOLD 500.0//角速度最大有效值
#define QUANTITY_AXE 'x'

extern Controler controler;

static unsigned char Re_buf[11], counter = 0;
static unsigned char sign = 0;
static bool first = true;//用于定性检测函数，是否是第一次传回加速度数据，用来判断以设置加速度初始值
static bool qfirst = true;
static bool cfirst = true;//用于初次校准
static float a[3], w[3], angle[3];

Gesture::Gesture(Device *device) {
  Serial.println("gesture constructing...");
  this->device = device;
  Serial.print("gesture constructed: ");
  Serial.println("device^" + this->device->getName() + " gestdata^NULL");
}

void Gesture::initial() {
  Serial.println("gesture.initializing...");
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
  }
  for (int i = 0; i < 3; i++) {
    Serial1.write(zzero[i]);
  }
  Serial.println("z-zeroing");

  byte acheck[3] = {0xFF, 0xAA, 0x67};
  for (int i = 0; i < 3; i++) {
    Serial1.write(acheck[i]);
  }
  for (int i = 0; i < 3; i++) {
    Serial1.write(acheck[i]);
  }
  Serial.println("A-calibration");
}

void Gesture::setGest_data(Gest_Data *gest_data) {
  this->gest_data = gest_data;
  if (this->gest_data == NULL) {
    Serial.println("gesture: device^" + this->device->getName() + " gestdata^NULL");
  } else {
    Serial.println(
      "gesture: device^" + this->device->getName() + " gestdata^" + "equation^" + this->gest_data->equation);
  }
}

Gest_Data *Gesture::getGest_data() {
  return this->gest_data;
}

Gest_Data *Gesture::detect() {
  Serial.println("gesture detecting...");
  String equation = "";
  float xa0 = ABSOLU_XA0;
  float ya0 = ABSOLU_YA0;
  float za0 = ABSOLU_ZA0;
  while (controler.isPressing()) {
      //    if (cfirst) {
      //      byte zzero[3] = {0xFF, 0xAA, 0x52};
      //      for (int i = 0; i < 3; i++) {
      //        Serial1.write(zzero[i]);
      //      }
      //      for (int i = 0; i < 3; i++) {
      //        Serial1.write(zzero[i]);
      //      }
      //      //    Serial.println("z-zeroing");
      //
      //      byte acheck[3] = {0xFF, 0xAA, 0x67};
      //      for (int i = 0; i < 3; i++) {
      //        Serial1.write(acheck[i]);
      //      }
      //      for (int i = 0; i < 3; i++) {
      //        Serial1.write(acheck[i]);
      //      }
            //    Serial.println("A-calibration");
      
      //      cfirst = false;
            //delay(2000);
      //    }

    //获取数据
    serialEvent();
    if (sign) { //若收到数据信号
      //Serial.println("get sign.");
      sign = 0;
      //解析数据信号
      if (Re_buf[0] == 0x55 && Re_buf[1] == 0x51) { //检查帧头，识别到加速度包
        //Serial.println("acceleration package gotten.");
        a[0] = (short(Re_buf[3] << 8 | Re_buf[2])) / 32768.0 * 16;//x
        a[1] = (short(Re_buf[5] << 8 | Re_buf[4])) / 32768.0 * 16;//y
        a[2] = (short(Re_buf[7] << 8 | Re_buf[6])) / 32768.0 * 16;//z
      } else {
        //Serial.println("acceleration package not gotten");
        continue;
      }

      if (first) { //设置加速度初始值
        Serial.println("acceleration initial set.");
        xa0 = a[0];
        ya0 = a[1];
        za0 = a[2];
        first = false;
        //                delay(4);
        Serial.println("xa0:" + String(xa0) + " ya0:" + String(ya0) + " za0:" + String(za0));
      } else {
        //转换数据信号为手势表达式元素
        String t = "";
         switch(pos_axe(xa0, ya0, za0)){
          case 1: t = "x+";break;
          case 2: t = "y+";break;
          case 3: t = "z+";break;
          case -1: t = "x-";break;
          case -2: t = "y-";break;
          case -3: t = "z-";break;
          case 0: t = "";
         }
        Serial.println("xa:" + String(a[0]) + " ya:" + String(a[1]) + " za:" + String(a[2]));
        equation += t;
        simplify(&equation);
        //Serial.println("gesture recorded: " + equation);
      }
    }
  }

  first = true;
//  cfirst = true;

  static Gest_Data t_gest = Gest_Data(equation, this->device);
  t_gest = Gest_Data(equation, this->device);//tododuan
  setGest_data(&t_gest);
  Serial.println("get gestdata: as above.");
  return this->gest_data;
}

Order *Gesture::analyze(Gest_Data *gest_data) {
  Serial.println("gesture analyzing...");
  if (gest_data->equation.equals("")) {
    Serial.println("getorder NULL.");
  } else {
    //打开文件：gest_data.device
    Serial.println("open corresponding device\'s file");
    if (SD.begin(SD_PIN)) {
      Serial.println("in");
      File file = SD.open(gest_data->device->getInfos().name(), FILE_READ);
      if (file) {
        Serial.println("opened.");
        //寻找手势表达式：gest_data.equation
        Serial.println("in file: gesture searching...");
        String data_gesture = "";
        String data_order = "";
        //加载首个orderType
        while (file.peek() != byte(0xFF) && file.available()) {
          data_order += char(file.read());
        }
        //进入循环，不断检测
        for (int i = 0, j = 0; file.available();) { //i是对于#的一个counter,j是对于\n的一个counter
          //检测0xFF
          if (file.read() == byte(0xFF)) {
            i++;
          }
          //每两个0xFF
          if (i % 2 == 0) {
            //加载gesture.equation
            while (file.peek() != byte('\r') && file.available()) {
              data_gesture += char(file.read());
            }
            if (file.available()) {
              file.read();//跳过\r
              file.read();//跳过\n
            }
            //判断gesture是否匹配
            gest_data->equation.trim();
            data_gesture.trim();

            Serial.println(gest_data->equation);
            Serial.println(data_gesture);
            //            for(int i = 0;i<7; i++){
            //              Serial.print(gest_data->equation.charAt(i));
            //              Serial.print(" ");
            //            }
            //            Serial.println();
            //            for(int i = 0;i<7; i++){
            //              Serial.print(data_gesture.charAt(i));
            //              Serial.print(" ");
            //            }
            //            Serial.println();
            Serial.println(gest_data->equation.charAt(6) == '\0');
            Serial.println(data_gesture.charAt(6) == '\0');
            //                        Serial.print(char('\0' + 13));
            Serial.println(gest_data->equation.compareTo(data_gesture));

            if (gest_data->equation.equals(data_gesture)) {
              Serial.println("in file: gesture found.");
              static Order ex_temp_order = Order(gest_data->device, data_order);//todo 清理对象累积
              ex_temp_order = Order(gest_data->device, data_order);
              file.close();
              Serial.println("getorder: device^" + ex_temp_order.getDevice()->getName() + " orderType^" +
                             ex_temp_order.getOrderType());
              return &ex_temp_order;
            }
            data_order = "";
            data_gesture = "";
            //加载orderType
            while (file.peek() != byte(0xFF) && file.available()) {
              data_order += char(file.read());
            }
          }
        }
        file.close();
        Serial.println("file closed.");
        Serial.println("getorder NULL.");
        return NULL;
      }
    }
  }
  return NULL;
}

void Gesture::quantity_detect(Order *order) {
  Serial.println("quantity gesture detecting...");//todo master
  float angle0 = ABSOLU_ANGLE0;
  while (controler.isPressing()) {
      //    if (cfirst) {
      //      byte zzero[3] = {0xFF, 0xAA, 0x52};
      //      for (int i = 0; i < 3; i++) {
      //        Serial1.write(zzero[i]);
      //      }
      //      for (int i = 0; i < 3; i++) {
      //        Serial1.write(zzero[i]);
      //      }
      //      //    Serial.println("z-zeroing");
      //
      //      byte acheck[3] = {0xFF, 0xAA, 0x67};
      //      for (int i = 0; i < 3; i++) {
      //        Serial1.write(acheck[i]);
      //      }
      //      for (int i = 0; i < 3; i++) {
      //        Serial1.write(acheck[i]);
      //      }
      //      //    Serial.println("A-calibration");
      //
      //      cfirst = false;
      //    }
    //获取数据
    serialEvent();
    if (sign) { //若收到数据信号
                                           //   Serial.println("get sign.");
      sign = 0;
      //解析数据信号
      if (Re_buf[0] == 0x55
          && Re_buf[1] == 0x53) { //检查帧头，识别到角度包
        //                Serial.println("Angle package gotten.");
        angle[QUANTITY_AXE - 120] =
          (short(Re_buf[(QUANTITY_AXE - 120) * 2 + 3] << 8 | Re_buf[(QUANTITY_AXE - 120) * 2 + 2])) /
          32768.0 * 180;
        if (qfirst) { //设置角度初始值
          //                    Serial.println("set w initial.");
          angle0 = angle[QUANTITY_AXE - 120];
          qfirst = false;
        }
    }
      //            else if (Re_buf[0] == 0x55
      //                       && Re_buf[1] == 0x52) { //检查帧头，识别到角速度包
      //                Serial.println("w package gotten.");
      //                w[QUANTITY_AXE - 120] =
      //                        (short(Re_buf[(QUANTITY_AXE - 120) * 2 + 3] << 8 | Re_buf[(QUANTITY_AXE - 120) * 2 + 2])) /
      //                        32768.0 * 2000;
      //                //如果角速度太快则挂起程序
      //                if (abs(w[QUANTITY_AXE - 120]) >= WTHRESHOLD) {
      //                    Serial.println("w is too fast, hang the program.");
      //                    qfirst = true;
      //                    continue;
      //                }
      //            }
      else {
        continue;
      }

                                              //  if (!qfirst && abs(w[QUANTITY_AXE - 120]) < WTHRESHOLD) {
      if (!qfirst) {
        //计算角度偏离量
        //                Serial.println("angle deviation calculating...");
        float deviation = angle[QUANTITY_AXE - 120] - angle0;
        deviation = deviation < -10.0 ? 180.0 + deviation : deviation;
        //发送角度
        Gest_Quantity_Data temp_qgest = Gest_Quantity_Data(deviation, order, this->device);
        controler.send(this->quantity_analyze(&temp_qgest));
        //                Serial.println("get_gest_quantity_order: as above.");
      }
    }
  }
//  cfirst = true;
  qfirst = true;
}

Order *Gesture::quantity_analyze(Gest_Quantity_Data *gest_quantity_data) {
  //填充device的.infos表格元素到成员变量中
  Serial.println("THREE ARRAY loading...");
  String *orderTypes = gest_quantity_data->device->getOrderTypes();
  Code *codings = gest_quantity_data->device->getCodings();
  Gest_Data *gestures = gest_quantity_data->device->getGestures();
  Serial.println("THREE ARRAY loaded. WOW! How scary!");
  /*
      1.对应到相应定量类型
      2.后检测相应的角度所处的范围并记录下标pos
      3.后根据下标找到return_order的各个成员变量
      4.返回return_order

      a.偏离量超出的问题，说明角速度太大造成校准值出现问题，积分紊乱无法避免，误差允许18°范围考虑，误差不允许忽略不计
      b.偏离量负值的问题，反向对应到180+负值
  */

  int pos = 0;//计位器
  Serial.println("quantitative type corresponding...");
  for (; pos < gest_quantity_data->device->getOrderNum(); pos++) {
    if (orderTypes[pos].equals(gest_quantity_data->order->getOrderType())) {
      Serial.println("quantitative type corresponded.");
      break;
    } else {
      Serial.println("not corresponded...");
    }
  }
  pos++;
  Serial.println("angle matching range finding...");
  for (; pos < gest_quantity_data->device->getOrderNum() && gestures[pos].equation.indexOf('~') > 2; pos++) {

    //分割字符串
    Serial.println("string splitting...");
    int j = gestures[pos].equation.indexOf('~');
    String min_s = "";
    String max_s = "";
    for (int p = 0; p < j; p++) {
      min_s += gestures[pos].equation.charAt(p);
    }
    for (int p = j + 1; p < gestures[pos].equation.length(); p++) {
      max_s += gestures[pos].equation.charAt(p);
    }
    Serial.println("string split.");

    //判断相应的角是否处在对应范围
    float min_d = min_s.toFloat();
    float max_d = max_s.toFloat();
    if (gest_quantity_data->angle >= min_d && gest_quantity_data->angle <= max_d) {
      Serial.println("range corresponded.");
      static Order temp_quantity_order = Order(gest_quantity_data->device, orderTypes[pos]);
      temp_quantity_order = Order(gest_quantity_data->device, orderTypes[pos]);
      Serial.println("get_quantity_order: as above.");
      return &temp_quantity_order;
    } else {
      Serial.println("range not corresponded.");
    }
  }
  Serial.println("get_quantity_order NULL.");
  return NULL;

}

void Gesture::serialEvent() {
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

void Gesture::simplify(String *s) {
  int len = s->length();
  int *f = (int *) malloc(6 * sizeof(int));
  char *c = (char *) malloc(len * sizeof(char));
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
      (*s) += c[i];
  }
  free(f);
  free(c);
}

/**
* 返回1/2/3/-1/-2/-3分别对应x+/y+/z+/x-/y-/z-, 返回0为全阴性或者无法判断
*/
byte Gesture::pos_axe(float xa0, float ya0, float za0){
  adx = abs(a[0]-xa0);
  ady = abs(a[1]-ya0);
  adz = abs(a[2]-za0);

  // 三轴变化都小于阈值，没有阳性轴
  if(adx < DAXTHRESHOLD && ady < DAYTHRESHOLD && adz < DAZTHRESHOLD){
      return 0;
  }
  //有且只有一轴变化大等于阈值，直接返回该轴
  else if (adx < DAXTHRESHOLD && ady < DAYTHRESHOLD){
      return a[2]-za0 > 0 ? 3 : -3;
  }else if (ady < DAYTHRESHOLD && adz < DAZTHRESHOLD){
      return a[0]-xa0 > 0 ? 1 : -1;
  }else if(adx < DAXTHRESHOLD && adz < DAZTHRESHOLD){
      return a[1]-ya0 > 0 ? 2 : -2;
  }
  //有且只有两轴变化大等于阈值，做出判断
  else if (adx < DAXTHRESHOLD){
      if(ady > adz){
        return a[1]-ya0 > 0 ? 2 : -2;
      }else if(ady < adz){
        return a[2]-za0 > 0 ? 3 : -3;
      }else{
        return 0;
      }
  }else if (ady < DAYTHRESHOLD){
    if(adx > adz){
      return a[0]-xa0 > 0 ? 1 : -1;
    }else if(adx < adz){
      return a[2]-za0 > 0 ? 3 : -3;
    }else{
      return 0;
    }
  }else if (adz < DAZTHRESHOLD){
    if(adx > ady){
      return a[0]-xa0 > 0 ? 1 : -1;
    }else if(adx < ady){
      return a[1]-ya0 > 0 ? 2 : -2;
    }else{
      return 0;
    }
  }
  //全部轴变化大等于阈值，做出判断
  else{
    if(adx == ady && ady == adz){//如果三个值相等
      return 0;
    }else if(adx > ady && adx > adz){
      return a[0]-xa0 > 0 ? 1 : -1;
    }else if(ady > adx && ady > adz){
      return a[1]-ya0 > 0 ? 2 : -2;
    }else if (adz > adx && adz > ady){
      return a[2]-za0 > 0 ? 3 : -3;
    }else{
      return 0;
    }
  }
}