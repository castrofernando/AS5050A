//////////////////////////////////////////////////////////////////////////
// AS5050A written by Fernando C. Castro (castro.fernando@me.com)	//
// Github: https://github.com/castrofernando/AS5050A.git		//
//////////////////////////////////////////////////////////////////////////

#ifndef AS5050A_H
#define AS5050A_H

//Command
#define CMD_READ                  (0x8000)
#define CMD_WRITE                 (0x0000)

// Chip Registers
#define POWER_ON_RESET            (0x3F22)
#define SOFTWARE_RESET            (0x3C00) 
#define MASTER_RESET              (0x33A5)
#define CLEAR_ERROR_FLAG          (0x3380)
#define NOP                       (0x0000)
#define AUTOMATIC_GAIN_CONTROL    (0x3FF8)
#define ANGULAR_DATA              (0x3FFF)
#define ERROR_STATUS              (0x33A5)
#define SYSTEM_CONFIG             (0x3F20)

#define AS5050A_RESOLUTION        1024 //10 bits angular data

#define MAX_ALARM_POINT           10 //max qty of alarm points set by user
#define ROUND_POINT_ERROR         5 // compare value +5 or -5 when compare alarm set points with absolute angle

#define ALM_HIGH    (1<<12)  // bit 12: Alarm flag, which indicates a too strong magnetic field
#define ALM_LOW     (1<<13)  // bit 13: Alarm flag, which indicates a too weak magnetic field

class AS5050A{
  public:
    AS5050A(int csPin,int mosi, int miso, int sck, uint32_t spi_speed);
    void begin();
    void setStartPosition(); //define a offset for readValue / read angle / totalAngle
    int32_t readValue();   //read scale from 0 to 1023 (10 bits)
    int16_t getAngle();    //convert value read from 0° up to 360° 
    int8_t getRotations(); //get rotation number
    int32_t getAbsoluteAngle(); //get absolute angle;
    void setAlarmPoint(int32_t absoluteAngle);

    typedef void (*alarmCallback)(int32_t);
     alarmCallback _callbackAlarm;
    void alarmEvent(alarmCallback alarm_callback);
  private:
    int32_t last_value;
    int32_t last_angle;
    int8_t rotation;
    uint8_t _csPin, _mosi, _miso, _sck;
    int32_t initial_value_offset;
    uint32_t _spi_speed;
    
    uint8_t str_ptr_counter=0;
    struct str_alarm_point{
      bool is_set = false;
      int32_t value = 0;
    };
    str_alarm_point alarm_point_array[MAX_ALARM_POINT] = {};

    int16_t spiSend(uint16_t reg_data, uint16_t type);
    uint8_t calcEvenParity(ushort value);
};

#endif