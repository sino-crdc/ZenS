#if ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Gesture.h"

#define DAXTHRESHOLD 0.01//x轴加速度可标志临界变化量
#define DAYTHRESHOLD 0.01//y轴加速度可标志临界变化量
#define DAZTHRESHOLD 0.01//z轴加速度可标志临界变化量
#define ABSOLU_XA0 0.1//x轴绝对初始加速度
#define ABSOLU_YA0 0.1//y轴绝对初始加速度
#define ABSOLU_ZA0 1.1//z轴绝对初始加速度
#define ABSOLU_ANGLE0 0.0//初始绝对角度
#define WTHRESHOLD 50.0//角速度最大有效值

extern Controler controler;

//todo a w angle 的类型
static unsigned char Re_buf[11], counter = 0;
static unsigned char sign = 0;
static unsigned char wait = 50;
static bool first = true;//用于定性检测函数，是否是第一次传回加速度数据，用来判断以设置加速度初始值
static bool qfirst = true;
static float a[3], w[3], angle[3];

//Arduino对基于对象的支持不是很好，private static 变量，声明时不可初始化挺正常，但是异文件类外定义竟然与private冲突，指定作用域也不行；同文件类外定义竟然还多重定义而不行！于是拉出
static SoftwareSerial sserial = SoftwareSerial(GEST_TX_PIN, GEST_RX_PIN);//9,10

Gesture::Gesture(Device* device) {
  Serial.println("gesture constructing...");
  this->device = device;
  Serial.print("gesture constructed: ");
  Serial.println("device^" + this->device->getName() + " gestdata^NULL");
}

void Gesture::initial() {
  Serial.println("gesture.initializing...");
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
}

void Gesture::setGest_data(Gest_Data* gest_data) {
  this->gest_data = gest_data;
  if (this->gest_data == NULL) {
    Serial.println("gesture: device^" + this->device->getName() + " gestdata^NULL");
  } else {
    Serial.println("gesture: device^" + this->device->getName() + " gestdata^" + "equation^" + this->gest_data->equation);
  }
}

Gest_Data* Gesture::getGest_data() {
  return this->gest_data;
}

Gest_Data* Gesture::detect() {
  Serial.println("gesture detecting...");
  String equation = "";
  while (controler.isPressing()) {
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
        Serial.println("gesture recorded: " + equation);
      }
    }
    delay(50+(wait++)%50);//判断循环条件->进入serialEvent->输出一个available，未执行完便直接开始新的循环(此函数)
  }

  first = true;

  static Gest_Data t_gest = Gest_Data(equation, this->device);
  setGest_data(&t_gest);
  Serial.println("get gestdata: as above.");
  return this->gest_data;
}

Order* Gesture::analyze(Gest_Data* gest_data) {
  Serial.println("gesture analyzing...");
  if (gest_data->equation.equals("")) {
    Serial.println("getorder NULL.");
  } else {
    //打开文件：gest_data.device
    Serial.println("open corresponding device\'s file");
    if (SD.begin(SD_PIN)) {
      File file = SD.open(gest_data->device->getInfos().name(), FILE_READ);
      if (file) {
        Serial.println("opened.");
        //寻找手势表达式：gest_data.equation
        Serial.println("in file: gesture searching...");
        String data_gesture = "";
        String data_order = "";
        //加载首个orderType
        while (file.peek() != byte('#') && file.available()) {
          data_order += char(file.read());
        }
        //进入循环，不断检测
        for (int i = 0, j = 0; file.available();) { //i是对于#的一个counter,j是对于\n的一个counter
          //检测'#'
          if (file.read() == byte('#')) {
            i++;
          }
          //每两个'#'
          if (i % 2 == 0) {
            //加载gesture.equation
            while (file.peek() != byte('\n') && file.available()) {
              data_gesture += char(file.read());
            }
            file.read();//跳过\n
            //判断gesture是否匹配
            if (gest_data->equation.equals(data_gesture)) {
              Serial.println("in file: gesture found.");
              static Order ex_temp_order = Order(gest_data->device, data_order);
              file.close();
              Serial.println("getorder: device^" + ex_temp_order.getDevice()->getName() + " orderType^" + ex_temp_order.getOrderType());
              return &ex_temp_order;
            }
            data_order = "";
            data_gesture = "";
            //加载orderType
            while (file.peek() != byte('#') && file.available()) {
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

Gest_Quantity_Data* Gesture::quantity_detect(Order* order) {
  Serial.println("quantity gesture detecting...");//todo master
  while (controler.isPressing()) {
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
        static Gest_Quantity_Data temp_gest_quantity_data = Gest_Quantity_Data(deviation, order, this->device);
        Serial.println("get_gest_quantity_order: as above.");
        return &temp_gest_quantity_data;
      }
    }
  }
}

Order* Gesture::quantity_analyze(Gest_Quantity_Data* gest_quantity_data) {
  //填充device的.infos表格元素到成员变量中
  Serial.println("THREE ARRAY loading...");
  String* orderTypes = gest_quantity_data->device->getOrderTypes();
  Code* codings = gest_quantity_data->device->getCodings();
  Gest_Data* gestures = gest_quantity_data->device->getGestures();
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
    String  min_s = "";
    String  max_s = "";
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
