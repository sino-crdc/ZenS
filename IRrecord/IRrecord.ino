#include <IRremote.h>
#include <SPI.h>
#include <SD.h>  //引入

File myFile;

int RECV_PIN = 11;  //在11口

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
  Serial.println("Creating my documents...");
  myFile = SD.open("my_documents", FILE_WRITE);
  Serial.println("my_documents is created");
}

void loop() {

  if(myFile){
      if (irrecv.decode(&results)) {
           Serial.println("Received a code, saving as raw");
           myFile.print(results.value, RAW);
           myFile.print("#");
           irrecv.resume(); }// Receive the next value
      delay(100);}
}
