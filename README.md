# AS5050A

# Description

Library based on arduino framework to comunicate with a SPI magnetic rotary encoder low power AS5050A.

# Tested hardwares

ESP32-WROOM

# How to use

1. Include de library AS5050A.h
2. Create a AS5050A object: AS5050A encoder(SS,MOSI,MISO,SCK,1000000); ;  -> Define here the SPI chip pinout and the SPI SPEED. The sensor SPI support rates up to 10MHz.
3. Use method begin() to initialize the device.

# methods available

Method | Description
_____________________
    void begin() | Initialize the spi sensor.
    <b>void setStartPosition()</b>  -> define a offset for readValue / read angle / totalAngle. Set a home  position to the device (value 0)
    <b>int32_t readValue()</b> -> read scale from 0 to 1023 (10 bits)
    <b>int16_t getAngle()</b>  -> convert value read from 0° up to 360° 
    <b>int8_t getRotations()</b> get rotation number (negative and positive to inform the direction)
    <b>int32_t getAbsoluteAngle()</b> -> get absolute angle based on rotations (negative and positive to indicate the direction);
    <b>void setAlarmPoint(int32_t absoluteAngle)</b> -> define a alarm point to be triggered by absoluteAngle. To this works, should read the value and absolute angle with some often (1ms? / 10ms?)
	
    <b>void alarmEvent(alarmCallback alarm_callback)<b> -> Alarm callback event. Based on the alarm points set, it will trigger an alarm when between +-5 value set.
	
	*** To define max alarm points, please change the define on .h file ->  #define MAX_ALARM_POINT . Default is 10.
	*** To change the interval alarm value, please change the define on .h file -> #define ROUND_POINT_ERROR . Default is 5.

# Simple example

```

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

```
