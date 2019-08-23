#include <IRremote.h>
#include <SPI.h>
#include <SD.h>//引入

#define SD_PIN 10//mega: 53
#define MOSI 11//51
#define MISO 12//50
#define SCK 13//52

File myFile;

int RECV_PIN = 7;  

IRrecv irrecv(RECV_PIN);

decode_results results;

union xx{
  unsigned int num;
  byte buf[2];  
}code;

void setup()
{
  Serial.begin(9600);
  // Open serial communications and wait for port to open:
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");//设置好红外接收器
  Serial.print("Initializing SD card...");
  pinMode(53,OUTPUT);
  if (!SD.begin(47)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("Creating my documents...");
  if(SD.exists("ac.txt"))
    SD.remove("ac.txt");
  myFile=SD.open("acc.txt",FILE_WRITE);
  Serial.println("my_documents is created");
  Serial.println(SD.exists("acc.txt"));
  myFile.close();
}

void loop() {
  myFile=SD.open("ac.txt",FILE_WRITE);
  if(myFile){
      if (irrecv.decode(&results)) {
           Serial.println("Received a code, saving as raw");
           dump(&results);
           irrecv.resume(); 
           }// Receive the next value
      delay(1000);
      }
   myFile.close();
   myFile=SD.open("ac.txt",FILE_READ);      
   while(myFile.available()){
    Serial.print(myFile.read(),HEX),Serial.print(" ");
   }
   Serial.println();
   myFile.close();
}

void dump(decode_results * results)
{
  myFile.print("switch#");
  int t = results->rawlen;
  Serial.print(t-1);
  Serial.print(':');
  for(int i=1;i<t;i++){
      code.num=results->rawbuf[i]*USECPERTICK;
      for(int j=0;j<2;j++)
        myFile.write(code.buf[1-j]);
      Serial.print(code.num,HEX);
      if(i<t-1) Serial.print(',');
    }
  myFile.print("#x-y-z-");
  Serial.println();
}
