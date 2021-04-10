//////////////////////////////////////////////////////////////////////////
// AS5050A written by Fernando C. Castro (castro.fernando@me.com)	//
// Github: https://github.com/castrofernando/AS5050A.git		//
//////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <AS5050A.h>

//SPI PINOUT - ESP32-WROOM

#define MISO   (gpio_num_t)19
#define MOSI   (gpio_num_t)23
#define SCK    (gpio_num_t)18
#define SS     (gpio_num_t)5

//Define encoder and spi pins
AS5050A encoder(SS,MOSI,MISO,SCK,1000000); 

//Initialize hardware
void initHardware(){
  pinMode(SS,OUTPUT);
  pinMode(MISO,INPUT);
  pinMode(MOSI,OUTPUT);
  pinMode(SCK,OUTPUT);
  digitalWrite(SS,HIGH);

  Serial.begin(9600);
}

//Alarm callback
void alarmEventCallback(int32_t alarm){
    Serial.print("ALARM CALLBACK: ");
    Serial.println(alarm);
}

void setup(){
    initHardware();
    encoder.begin();
    encoder.setAlarmPoint(450);		//set and event when absolute event between 445 and 455 (+- 5 -> see as5050a.h file)
    encoder.setAlarmPoint(-350);	//set and event when absolute event between -355 and -345 (+- 5 -> see as5050a.h file)
    encoder.alarmEvent(&alarmEventCallback); //pass a function address to be called by alarm event callback.
}

void loop(){
      delay(1);
      Serial.print("Value: ");
      Serial.print(encoder.readValue(),DEC); //need to be call to all others methods work.
      Serial.print("\t Angle: ");
      Serial.print(encoder.getAngle(),DEC);
      Serial.print("\t Rotations: ");
      Serial.print(encoder.getRotations(),DEC);
      Serial.print("\t Absolute Angle: ");
      Serial.println(encoder.getAbsoluteAngle(),DEC); //need to call to be able treat the alarm event.
}

