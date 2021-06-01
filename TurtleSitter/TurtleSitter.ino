#include <SoftwareSerial.h>
#include <Servo.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include "U8glib.h"

#define rx 10
#define tx 11
#define coldPin 2
#define hotPin 3
#define temp 1
#define coldTemp 18
#define hotTemp 30

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE); 
OneWire oneWire(temp);
DallasTemperature sensor(&oneWire);
SoftwareSerial BTSerial(rx, tx);

Servo servo;
int celsius=0;
char inputByte;
int pos = 0;
long timer = 100000; 
long aux_timer=0;
long endtime; 
long now;
long fixed_now;
long aux_now_timer;
long aux_now_manual;
int limit=0;
int buttonPressed=0;
int manualfeed=0;
int timer_change_start=0;
int timer_change_end=0;
int cancel=0;

void setup(){

  pinMode(coldPin,OUTPUT);
  pinMode(hotPin,OUTPUT);
  pinMode(temp,INPUT_PULLUP);
  BTSerial.begin(9600);
  servo.write(0);
  delay(15);
  sensor.begin();

}

void loop(){
  //calcul milisecunde trecute de la pornire
  now = millis();

  //scriere pe ecran LCD
  u8g.firstPage();  
  do {
    if(manualfeed)  {
      aux_now_manual = now + 5000;
      manualfeed=0;
    }
    else if(now < aux_now_manual)
      drawManualFeed();

    else if (timer_change_end) {
      aux_now_timer = now+5000;
      timer_change_end=0;
    }
    else if(now<aux_now_timer)
      drawTimerChanged();
    else if(now/1000%10>=5)
      drawTemp();
    else if(now/1000%10>=0) {
      if(now/1000%10==0)
        fixed_now=now;
      drawTime();
    }
      
  } while(u8g.nextPage());
  
  //servo porneste dupa timer
  if(now > endtime) {
    BTSerial.println("Feeder started by timer!");
    BTSerial.println("Timer reset");
    servo.attach(8);
    endtime = now + timer;
    for(pos=0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
      {                                  // in steps of 1 degree 
        servo.write(pos);            // tell servo to go to position in variable 'pos' 
        delay(15);                       // waits 15ms for the servo to reach the position 
      } 
      for(pos=180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
      {                                
        servo.write(pos);              // tell servo to go to position in variable 'pos' 
        delay(15);                       // waits 15ms for the servo to reach the position 
      }
       servo.detach();
    
  }
  //input de la telefon
  else if(BTSerial.available()>0){

    inputByte= BTSerial.read();
    //servo porneste de la comanda
    if (inputByte=='Z'){
      manualfeed=1;
      BTSerial.println("Manually started feeder!");
      BTSerial.println("Timer reset");
      servo.attach(8);
      endtime = now + timer;
      for(pos; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
      {                                  // in steps of 1 degree 
        servo.write(pos);             // tell servo to go to position in variable 'pos' 
        delay(15);                       // waits 15ms for the servo to reach the position 
      } 
      for(pos; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
      {                                
        servo.write(pos);             // tell servo to go to position in variable 'pos' 
        delay(15);                       // waits 15ms for the servo to reach the position 
      }
      servo.detach();
    }
    //semmnal schimbare timer
    else if(inputByte=='t') {
      timer_change_start=1;
      BTSerial.println("How much time between feeds? (in hours)");
      
    }
    //numar ce reprezinta numarul de ore pt timer
    else if(inputByte>='0' && inputByte<='9' && timer_change_start) {
     
      aux_timer = aux_timer*10 + (inputByte-'0');
       if(BTSerial.available()>0) {
        inputByte=BTSerial.read();
        if(inputByte>='0' && inputByte<='9')
          aux_timer = aux_timer*10 + (inputByte-'0');
      }
     
      BTSerial.println(aux_timer);
      timer=aux_timer*3600000;
      aux_timer=0;
      endtime = now + timer;
      timer_change_end=1;
      timer_change_start=0;
    }
  }
  //citire temperatura
   sensor.requestTemperatures(); 
      celsius=sensor.getTempCByIndex(0);
      //temperatura prea mare
      if(celsius >= hotTemp ) {
        limit=1;
        digitalWrite(hotPin, HIGH);
        BTSerial.print("Water too hot! ");
        BTSerial.print(celsius);
        BTSerial.println(" C");

      }
      //temperatura prea mica
      else if(celsius <= coldTemp ){
        limit=-1;
        digitalWrite(coldPin, HIGH);
        BTSerial.print("Water too cold! ");
        BTSerial.print(celsius);
        BTSerial.println(" C");
      }
      else {
        limit=0;
        digitalWrite(hotPin, LOW);
        digitalWrite(coldPin, LOW);
        BTSerial.print("Water temperature is OK: ");
        BTSerial.print(celsius);
        BTSerial.println(" C");
      }
      delay(1000);

}
  //scriere ecran cu temperatura
  void drawTemp(void) {
  u8g.setFont(u8g_font_9x15B);
  u8g.setPrintPos(0, 12); 
  u8g.println("Water Temp =");
  u8g.setPrintPos(0,30);
  int var = 0;
  u8g.print(celsius);
  u8g.print("C ");
  if(limit == 1) u8g.print("TOO HOT!");
  else if(limit == -1) u8g.print("TOO COLD!");

}
  //scriere ecran cu timpul pana la hranire
 void drawTime(void) {
  u8g.setFont(u8g_font_9x15B);
  u8g.setPrintPos(0, 12); 
  u8g.println("Time to feed =");
  u8g.setPrintPos(0,30);
  int var = 0;
  if((endtime-fixed_now)/60000 > 120) {
    u8g.print((endtime-fixed_now)/3600000);
    u8g.print(" h");
  }
  else {
    u8g.print((endtime-fixed_now)/60000);
    u8g.print(" min");
  }
}
  //scriere ecran cu mesaj ca s-a hranit manual
 void drawManualFeed(void) {
    u8g.setFont(u8g_font_9x15B);
    u8g.setPrintPos(0, 12);
    u8g.println("Manual feed!");
    u8g.setPrintPos(0,30);
    u8g.print ("Timer reset");
    manualfeed=0;
 }
  //scriere ecan cu mesaj ca s-a schimbat timerul
 void drawTimerChanged(void) {
    u8g.setFont(u8g_font_9x15B);
    u8g.setPrintPos(0, 12);
    u8g.print("Timer changed:");
    u8g.setPrintPos(0,30);
    u8g.print(timer/3600000);
    u8g.print(" h");
 }
