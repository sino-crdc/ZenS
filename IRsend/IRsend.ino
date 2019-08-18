/*
 * 暂时只发射一个指令
 */
#include <IRremote.h>
#include <SPI.h>
#include <SD.h>//引入
#include <IRremote.h>

IRsend irsend;
File myFile;
unsigned int * a;
int len=0;
int RECV_PIN=7;
char cc[10]={0};
int lencc=0;

void setup()
{
  pinMode(10,OUTPUT);
  Serial.begin(9600);
  if(!SD.begin(4)){
      Serial.println("SD initialization failed!");
    }
  Serial.println("SD initialization done");
  myFile=SD.open("acc.txt",FILE_READ);
  if(myFile){
    Serial.println("convertion begin");
    while(myFile.available()){
        //cc[lencc++]=(char)myFile.read();
        //Serial.print(cc[lencc-1]);
        if(cc[lencc-1]==':'){
            lencc--;
            len=tonum()-1;
            //Serial.print("len is ");
            //Serial.println(len);
            a=(unsigned int * )malloc(len*sizeof(unsigned int));
            len=0;
          }
        else if(cc[lencc-1]==','||cc[lencc-1]=='#'){
            lencc--;
            if(len==0){
               len++;
               lencc=0;
               continue;
              }
            *(a+len-1)=tonum();
            len++;
          }
      }
  }
  //Serial.println();
  myFile.close();
  //for(int i=0;i<len-1;i++)
    //Serial.print(a[i]),Serial.print(',');
  //Serial.println();
}

void loop()
{
  irsend.sendRaw(a,len-1,38);
  delay(2000);
}

unsigned int tonum()//将文件中的字符转化为数字
{
  unsigned int num=0;
  for(int i=0;i<lencc;i++)
    num=num*10+cc[i]-'0';
  lencc=0;
  return num;
}
