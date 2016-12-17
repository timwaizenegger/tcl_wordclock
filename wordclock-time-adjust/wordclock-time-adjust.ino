// Date and time functions using a RX8025 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "Sodaq_DS3231.h"

char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

//year, month, date, hour, min, sec and week-day(starts from 0 and goes to 6)
//writing any non-existent time-data may interfere with normal operation of the RTC.
//Take care of week-day also.
DateTime dt(2016, 16, 12, 22, 40, 0, 5);
// VERZÖGERUNG: ca. 6 sekunden auf arduino nano


void setup () 
{
    Serial.begin(57600);
    Wire.begin();
    rtc.begin();
    rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above 
}

void loop () 
{
    DateTime now = rtc.now(); //get the current date-time
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.date(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.print(weekDay[now.dayOfWeek()]);
    Serial.println();

  uint8_t time_now_h;
  uint8_t time_now_h12;
  uint8_t time_now_m;

  time_now_h = now.hour();
  time_now_h12 = time_now_h % 12;
  if(0 == time_now_h12) time_now_h12 = 12;
  time_now_m = now.minute();

  Serial.println("drawing time: m");
  Serial.println(time_now_m);
  Serial.println(time_now_h);
  Serial.println(time_now_h12);


    
    delay(1000);
}
