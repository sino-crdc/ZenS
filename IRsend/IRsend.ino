/*
 * 暂时只发射一个指令
 */
#include "IRremote2.h" 
#include <SPI.h>
#include <SD.h>//引入

IRsend irsend;
File myFile;
unsigned int * a;
int len=0;
int RECV_PIN=7;
union xx{
  unsigned int num;
  byte buf[2];
}code;

void setup()
{
  pinMode(53,OUTPUT);
  Serial.begin(4800);
  if(!SD.begin(4)){
      Serial.println("SD initialization failed!");
      while(1);
    }
  Serial.println("SD initialization done");
  myFile=SD.open("light.txt",FILE_READ);
  for(int i=0;i<7;i++)
    myFile.read();
  if(myFile){
      a=(unsigned int *)malloc(243*sizeof(unsigned int));
      while(myFile.available()){
          code.buf[1]=myFile.read();
          code.buf[0]=myFile.read();
          Serial.print((char)code.buf[1]);
          Serial.print(" ");
          Serial.print((char)code.buf[0]);
          Serial.print(" ");
          if(code.buf[1]!=0xFF&&code.buf[0]!=0xFF)
            a[len++]=code.num;
          else
            break;
        }
    }
  Serial.println();
  Serial.println(len);
  for(int i=0;i<len;i++)
    Serial.print(a[i]),Serial.print(" ");
  Serial.println();
}

void loop()
{
   irsend.sendRaw(a,len,38);
   delay(1000);
}
