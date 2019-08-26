#include <IRremote.h>
#include <SPI.h>
#include <SD.h>//引入


#define MOSI 11//mega:51
#define MISO 12//mega:50
#define SCK 13//mega:52

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
  if(SD.exists("light.txt"))
    SD.remove("light.txt");
  myFile=SD.open("light.txt",FILE_WRITE);
  Serial.println("my_documents is created");
  Serial.println(SD.exists("light.txt"));
  myFile.close();
}

void loop() {
  myFile=SD.open("light.txt",FILE_WRITE);
  if(myFile){
      if (irrecv.decode(&results)) {
           Serial.println("Received a code, saving as raw");
           dump(&results);
           irrecv.resume(); 
           }// Receive the next value
      delay(1000);
      }
   myFile.close();
   myFile=SD.open("light.txt",FILE_READ);      
   while(myFile.available()){
    Serial.print(myFile.read(),DEC),Serial.print(" ");
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
      Serial.print(code.num,DEC);
      if(i<t-1) Serial.print(',');
    }
  myFile.print("#y+y-");
  Serial.println();
}
