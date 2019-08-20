#include <IRremote.h>
#include <SPI.h>
#include <SD.h>//引入

#define SD_PIN 10
#define MOSI 11
#define MISO 12
#define SCK 13

File myFile;

int RECV_PIN = 9;  

IRrecv irrecv(RECV_PIN);

decode_results results;

union code{
    byte x[2];
    int num;
  }x;
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
  pinMode(10,OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("Creating my documents...");
  if(SD.exists("ac.txt"))
    SD.remove("ac.txt");
  myFile=SD.open("ac.txt",FILE_WRITE);
  Serial.println("my_documents is created");
  Serial.println(SD.exists("ac.txt"));
  myFile.close();
}

void loop() {
  myFile=SD.open("ac.txt",FILE_WRITE);
  if(myFile){
      if (irrecv.decode(&results)) {
           Serial.println("Received a code, saving as raw");
           dump(&results);
           irrecv.resume(); }// Receive the next value
      delay(1000);
      }
   myFile.close();
   myFile=SD.open("ac.txt",FILE_READ);      
   while(myFile.available()){
    Serial.print((char)myFile.read()),Serial.print(" ");
   }
   Serial.println();
   myFile.close();
}

void dump(decode_results * results)
{
  int t = results->rawlen;
  Serial.print(t);
  Serial.print(':');
  for(int i=1;i<t;i++){
      myFile.print(results->rawbuf[i]*USECPERTICK);
      if(i<t-1) myFile.print(',');
      Serial.print(results->rawbuf[i]*USECPERTICK);
      if(i<t-1) Serial.print(',');
    }
  myFile.print('#');
  Serial.println();
}
