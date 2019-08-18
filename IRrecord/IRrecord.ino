#include <IRremote.h>
#include <SPI.h>
#include <SD.h>  //引入

#define SD_PIN 10
#define MOSI 11
#define MISO 12
#define SCK 13

File myFile;

int RECV_PIN = 9;  //在11口

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  // Open serial communications and wait for port to open:
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");//设置好红外接收器
   
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");//配置好SD卡

  // open a new file 
  Serial.println("Creating ac.txt");
  myFile = SD.open("ac.txt", FILE_WRITE);
  Serial.println("ac.txt is created");

  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
}

void loop() {
  if(myFile){
      if (irrecv.decode(&results)) {
           Serial.println("Received a code, saving as raw");
           myFile.write(results.value, BIN);
           myFile.write("#");
           irrecv.resume(); 
      }// Receive the next value
      delay(100);
   }
}
