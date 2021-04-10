//////////////////////////////////////////////////////////////////////////
// AS5050A written by Fernando C. Castro (castro.fernando@me.com)	//
// Github: https://github.com/castrofernando/AS5050A.git		//
//////////////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "AS5050A.h"
#include "SPI.h"

#define DEBUG

    //Define SPI pinout and speed
    AS5050A::AS5050A(int csPin,int mosi, int miso, int sck, uint32_t spi_speed){
     _csPin = csPin;
     _mosi = mosi;
     _miso = miso;
     _sck = sck;
     _spi_speed = spi_speed;
     pinMode(_csPin, OUTPUT);
     pinMode(mosi,OUTPUT);
     pinMode(miso,INPUT);
     pinMode(sck,OUTPUT);
     digitalWrite(_csPin, HIGH);
   }

   //Start SPI and master reset the peripheral.Then, Set start position.
   void AS5050A::begin(){
     SPI.begin(_sck,_miso,_mosi,_csPin);
     SPI.setFrequency(_spi_speed);
     SPI.setBitOrder(SPI_MSBFIRST);
     SPI.setDataMode(SPI_MODE1);

     //master reset on being
     spiSend(MASTER_RESET,CMD_WRITE);
     SPI.end();

     //set home position
     rotation=0;
     setStartPosition();
   }

    //Send data by SPI passing the command and type (e.g: READ_ANGLE / CMD_READ)
    int16_t AS5050A::spiSend(uint16_t reg_data, uint16_t type){
     SPI.begin(_sck,_miso,_mosi,_csPin);
     SPI.setFrequency(_spi_speed);
     SPI.setBitOrder(SPI_MSBFIRST);
     SPI.setDataMode(SPI_MODE1);
     int16_t data = (reg_data<<1) | type;
     data |= calcEvenParity(data);
     digitalWrite(_csPin, LOW); 
     int16_t value = SPI.transfer16(data);
     digitalWrite(_csPin, HIGH);
     SPI.end();
     return value;
   }

   //define a offset for readValue / read angle / totalAngle
   void AS5050A::setStartPosition(){
     initial_value_offset = readValue();
     initial_value_offset = readValue();
     #ifdef DEBUG
      Serial.print("Initial value:");
     #endif
     Serial.println(initial_value_offset);
   }

   //read angular data - scale from 0 to 1023 (10 bits)
    int32_t AS5050A::readValue(){ 
     int32_t value = spiSend(ANGULAR_DATA, CMD_READ);
     value = spiSend(NOP, CMD_WRITE);
     if((value & 0x0002) != 0){
        #ifdef DEBUG
          Serial.println("EF error");
        #endif
        spiSend(MASTER_RESET,CMD_WRITE); // clear deadlock - need to master reset to back from a deadlock
     }
     if((value & ALM_LOW == 0) && (value & ALM_HIGH == 1)){
       #ifdef DEBUG
        Serial.println("AGC level is higher than the maximum level. The magnetic field is too weak.");   
       #endif
     }else if((value & ALM_LOW == 1) && (value & ALM_HIGH == 0)){
       #ifdef DEBUG
        Serial.println("AGC level is lower than the minimum level. The magnetic field is too strong.");   
       #endif
     }else if((value & ALM_LOW == 1) && (value & ALM_HIGH == 1)){
       #ifdef DEBUG
        Serial.println("indicates if a major system error has occurred during the last READ ANGLE command or if the WOW flag is active. During active WOW a READ ANGLE command must not be sent. Error flags can be read out with the error status register.");   
       #endif
     }
      value = (value>>2) & 0x03FF;

      if(value >= initial_value_offset){
        last_value=value - (initial_value_offset);
      }else{
        last_value = value + AS5050A_RESOLUTION - initial_value_offset;
      }

      int32_t angle = (last_value * 360) / (AS5050A_RESOLUTION);  

     //try to get rotations...//TODO: need to improve
     if(last_angle >= 300 && angle <=50) rotation+=1;
     else if (last_angle <=50 && angle >=300) rotation-=1;

      //record the latest angle
     last_angle = angle;

     return value;
   }

   //convert value read from 0° up to 360° 
   int16_t AS5050A::getAngle(){
     return last_angle;
   }

   //get absolute angle. Need to call this function to receive a alarm callback
  int32_t AS5050A::getAbsoluteAngle(){   
       int32_t result = (int32_t)(last_angle)+((int32_t)rotation * (int32_t)360);
       for (size_t i = 0; i < MAX_ALARM_POINT; i++)
       {
         if(alarm_point_array[i].is_set == false){
           break;
         }else{
           if((alarm_point_array[i].value >= (result -ROUND_POINT_ERROR)) && (alarm_point_array[i].value <= (result +ROUND_POINT_ERROR))){
             _callbackAlarm(alarm_point_array[i].value);           
           }
         }
       }    
       return result;      
   }

   //alarm event callback
   void AS5050A::alarmEvent(alarmCallback set_point_alarm){
     _callbackAlarm = set_point_alarm;
   }
     
    //get rotation count (positive and negative)
   int8_t AS5050A::getRotations(){
     if(rotation<0){
       return rotation+1; //small manual adjust \0/
     }
     return rotation;
   }

   //set up to 10 alarm points to receive as a callback
   void AS5050A::setAlarmPoint(int32_t absoluteAngle){
      if(str_ptr_counter<MAX_ALARM_POINT){
        alarm_point_array[str_ptr_counter].is_set=true;
        alarm_point_array[str_ptr_counter].value=absoluteAngle;
        str_ptr_counter++;
      }else{
        #ifdef DEBUG
          Serial.println("Max. alarm points reached.\r\n Please,increase #define MAX_ALARM_POINT in .h file. Standard are 10 inputs");
        #endif
      }
   }

 //create a even parity for transmition
 uint8_t AS5050A::calcEvenParity(ushort value) {    
    uint8_t cnt = 0;     
    uint8_t i;     
    for (i = 0; i < 16; i++)     
    { 
        if (value & 0x1)         
        {             
            cnt++;         
        }         
        value >>= 1;     
    }     
    return cnt & 0x1; 
}