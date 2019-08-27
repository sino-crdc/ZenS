#include <IRremote2.h>
#include <SPI.h>
#include <SD.h>//引入


#define MOSI 11//mega:51
#define MISO 12//mega:50
#define SCK 13//mega:52

File myFile;

int RECV_PIN = 3;

IRrecv irrecv(RECV_PIN);

decode_results results;

union xx {
  unsigned int num;
  byte buf[2];
} code;

void setup()
{
  Serial.begin(4800);
  // Open serial communications and wait for port to open:
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");//设置好红外接收器
  Serial.print("Initializing SD card...");
  pinMode(53, OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("Creating my documents...");
  if (SD.exists("light.txt"))
    SD.remove("light.txt");
  myFile = SD.open("light.txt", FILE_WRITE);
  Serial.println("my_documents is created");
  Serial.println(SD.exists("light.txt"));
  myFile.close();
}

int f = 1;

void loop() {
  myFile = SD.open("light.txt", FILE_WRITE);
  if (myFile) {
    if (irrecv.decode(&results)) {
      Serial.println("Received a code, saving as raw");
      switch (f) {
        case 1: dump1(&results);
          f++;
          break;
        case 2: dump2(&results);
          f++;
          break;
        case 3: dump3(&results);
          f++;
          break;
        case 4: dump4(&results);
          f++;
          break;
      }
      irrecv.resume();
    }// Receive the next value
    delay(1000);
  }
  myFile.close();
  myFile = SD.open("light.txt", FILE_READ);
  while (myFile.available()) {
    Serial.print(myFile.read(), DEC), Serial.print(" ");
  }
  Serial.println();
  myFile.close();
}

void dump1(decode_results * results)//switch
{
  myFile.print("switch");
  myFile.write(0xFF);
  int t = results->rawlen;
  Serial.print(t - 1);
  Serial.print(':');
  for (int i = 1; i < t; i++) {
    code.num = results->rawbuf[i] * USECPERTICK;
    for (int j = 0; j < 2; j++)
      myFile.write(code.buf[1 - j]);
    Serial.print(code.num, DEC);
    if (i < t - 1) Serial.print(',');
  }
  myFile.write(0xFF);
  myFile.print("y+y-\r\n");
  Serial.println();
}

void dump2(decode_results * results)//dimming
{
  myFile.print("$dimming");
  myFile.write(0xFF);
  int t = results->rawlen;
  Serial.print(t - 1);
  Serial.print(':');
  for (int i = 1; i < t; i++) {
    code.num = results->rawbuf[i] * USECPERTICK;
    for (int j = 0; j < 2; j++)
      myFile.write(code.buf[1 - j]);
    Serial.print(code.num, DEC);
    if (i < t - 1) Serial.print(',');
  }
  myFile.write(0xFF);
  myFile.print("z+\r\n");
  Serial.println();
}

void dump3(decode_results * results)//up
{
  myFile.print("$up");
  myFile.write(0xFF);
  int t = results->rawlen;
  Serial.print(t - 1);
  Serial.print(':');
  for (int i = 1; i < t; i++) {
    code.num = results->rawbuf[i] * USECPERTICK;
    for (int j = 0; j < 2; j++)
      myFile.write(code.buf[1 - j]);
    Serial.print(code.num, DEC);
    if (i < t - 1) Serial.print(',');
  }
  myFile.write(0xFF);
  myFile.print("0.0~90.0\r\n");
  Serial.println();
}

void dump4(decode_results * results)//down
{
  myFile.print("$down");
  myFile.write(0xFF);
  int t = results->rawlen;
  Serial.print(t - 1);
  Serial.print(':');
  for (int i = 1; i < t; i++) {
    code.num = results->rawbuf[i] * USECPERTICK;
    for (int j = 0; j < 2; j++)
      myFile.write(code.buf[1 - j]);
    Serial.print(code.num, DEC);
    if (i < t - 1) Serial.print(',');
  }
  myFile.write(0xFF);
  myFile.print("91.0~180.0\r\n");
  Serial.println();
}
