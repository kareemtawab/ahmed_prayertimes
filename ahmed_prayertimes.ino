#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "PrayerTimes.h"
#include "RTClib.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
RTC_DS3231 rtc;

float salatLatitude = 31.212387; //New Cairo
float salatLongitude = 29.946572; //New Cairo
int salatTimezone = 2;  //New Cairo
double salatTimes[sizeof(TimeName) / sizeof(char*)];
int salatHours;
int salatMinutes;
char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
float salatTimeinFloat = 00.00L;
float nowTimeinFloat = 00.00L;

byte AM[8] = {
  0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x00
};

byte PM[8] = {
  0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10, 0x00
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Prayer Times");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
  //Serial.println("RTC lost power, lets set the time!");
  // following line sets the RTC to the date & time this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  //rtc.adjust(DateTime(2019, 3, 16, 23, 30, 0));
  }

  lcd.backlight();
  lcd.init();                      // initialize the lcd
  lcd.createChar(0, AM);
  lcd.createChar(1, PM);

  set_calc_method(Egypt);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(19.5);
  set_isha_angle(17.5);
}


void loop() {
  delay(250);
  DateTime now = rtc.now();
  char buf[3];
  lcd.setCursor(0, 0);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(".");
  sprintf(buf, "%02d", now.day());
  lcd.print(buf);
  lcd.print("/");
  sprintf(buf, "%02d", now.month());
  lcd.print(buf);
  lcd.setCursor(15, 0);
  if (now.hour() >= 12) {
    lcd.write(byte(1));
  }
  else {
    lcd.write(byte(0));
  }
  lcd.setCursor(10, 0);
  if (now.hour() > 12) {
    sprintf(buf, "%02d", now.hour() - 12);
    lcd.print(buf);
    lcd.print(":");
    sprintf(buf, "%02d", now.minute());
    lcd.print(buf);
  }
  else {
    sprintf(buf, "%02d", now.hour());
    lcd.print(buf);
    lcd.print(":");
    sprintf(buf, "%02d", now.minute());
    lcd.print(buf);
  }


  get_prayer_times(now.year(), now.month(), now.day(), salatLatitude, salatLongitude, salatTimezone, salatTimes);
  for (int i = 0; i <= 5; i++) {
    if (i <= 3) {
      get_float_time_parts(salatTimes[i], salatHours, salatMinutes);
    }
    else {
      if (i == 4) {
        get_float_time_parts(salatTimes[5], salatHours, salatMinutes);
      }
      else {
        get_float_time_parts(salatTimes[6], salatHours, salatMinutes);
      }
    }
    salatTimeinFloat = float(salatHours) + float(salatMinutes) / 60.00;
    nowTimeinFloat = float(now.hour()) + float(now.minute()) / 60.00;
    if (salatTimeinFloat < nowTimeinFloat) {
    }
    else {
      lcd.setCursor(0, 1);
      if (i <= 3) {
        lcd.print(TimeName[i]);
      }
      else {
        if (i == 4) {
          lcd.print(TimeName[5]);
        }
        else {
          lcd.print(TimeName[6]);
        }
      }
      lcd.setCursor(10, 1);
      if (salatHours > 12) {
        sprintf(buf, "%02d", salatHours - 12);
        lcd.print(buf);
        lcd.print(":");
        sprintf(buf, "%02d", salatMinutes);
        lcd.print(buf);
      }
      else {
        sprintf(buf, "%02d", salatHours);
        lcd.print(buf);
        lcd.print(":");
        sprintf(buf, "%02d", salatMinutes);
        lcd.print(buf);
      }
      lcd.setCursor(15, 1);
      if (salatHours >= 12) {
        lcd.write(byte(1));
      }
      else {
        lcd.write(byte(0));
      }
      break;
    }
  }
  get_float_time_parts(salatTimes[6], salatHours, salatMinutes);
  if (salatTimeinFloat < nowTimeinFloat) {
    get_prayer_times(now.year(), now.month(), now.day() + 1, salatLatitude, salatLongitude, salatTimezone, salatTimes);
    get_float_time_parts(salatTimes[0], salatHours, salatMinutes);
    lcd.setCursor(0, 1);
    lcd.print(TimeName[0]);
    lcd.setCursor(10, 1);
    if (salatHours > 12) {
      sprintf(buf, "%02d", salatHours - 12);
      lcd.print(buf);
      lcd.print(":");
      sprintf(buf, "%02d", salatMinutes);
      lcd.print(buf);
    }
    else {
      sprintf(buf, "%02d", salatHours);
      lcd.print(buf);
      lcd.print(":");
      sprintf(buf, "%02d", salatMinutes);
      lcd.print(buf);
    }
    lcd.setCursor(15, 1);
    if (salatHours >= 12) {
      lcd.write(byte(1));
    }
    else {
      lcd.write(byte(0));
    }
  }
}
