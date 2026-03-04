/* 
 http://forum.rcl-radio.ru/viewtopic.php?id=87&p=5
 http://rcl-radio.ru/?p=67405
 https://oshwlab.com/allexsemenow/iv-6-clock_copy
 v.0 - changed for DS3231 by niq_ro (Nicu FLORICA)
 v.0a - v.0a - translate the comments in english and move the commands as one per line
 v.1 - replaced iarduino_RTC.h with RTClib.h library  // https://github.com/adafruit/rtclib
 v.2 - added temperature on the display (internal sensor from DS3231 chip)
*/

#define ADDR_INIT 6    // Адрес для проверки -> Address for verification
#define INIT_KEY 15    // Ключ проверки на первый запуск -> Verification key for first launch
#define ADDR_YRKOST 0  // Адрес хранения значения яркости -> Brightness value storage address
#define ADDR_SVET 2    // Адрес хранения состояния подсветки -> Backlight state storage address
#define PERIOD 1000
#define ADDR_TEMP 3    // store the state for show or not the temperature
#include "MsTimer2.h"  // // http://rcl-radio.ru/wp-content/uploads/2018/11/MsTimer2.zip 
#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"  // https://github.com/adafruit/rtclib
RTC_DS3231 rtc;
int an, segm, times, i, ss, mm, hh;
int dp;
byte a[5];  // масив содержащий выводимую на индикаторы информацию -> Array containing information displayed on the indicators
int menu, year, month, day, hour, minute, second, weekday, w, w2, save_t;
int menu_svet, svet_state, brightness, svet, w3;
uint32_t timer = 0;
int led_pwm = 10;
int dps;
int tt; 
int showt;
int showts = 55;
///////////// Управляющие сетки ВЛИ -> VFD control grids/////////////////////////
int grids_HL1 = 4;
int grids_HL2 = 3;
int grids_HL3 = 2;
int grids_HL4 = 13;
int grids_HL5 = A0;
////////////////////////// Сегменты ВЛИ ->  VFD segments ////////////////////////
int SEG_A = 7;
int SEG_B = 5;
int SEG_C = 6;
int SEG_D = 12;
int SEG_E = 11;
int SEG_F = 9;
int SEG_G = 8;
//////////////////////////////////// Кнопки -> Buttons //////////////////////
int SW1 = A7;
int SW2 = A6;
int SW3 = A2;
int SW4 = A1;

void setup() {
   //////////////////////// Настройка выводов микроконтроллера -> Microcontroller pin settings ////////////////
  pinMode(grids_HL1, OUTPUT);
  pinMode(grids_HL2, OUTPUT);
  pinMode(grids_HL3, OUTPUT);
  pinMode(grids_HL4, OUTPUT);
  pinMode(grids_HL5, OUTPUT);
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  
  MsTimer2::set(2, to_Timer);
  MsTimer2::start();
  Serial.begin(9600);
  Serial.println("-");
  Serial.println("-");  
  //Serial.end();  // Отключаем последовательный порт -> Disabling the serial port

  if (EEPROM.read(ADDR_INIT) != INIT_KEY) 
  
  {  // проверка на первый запуск -> First-run test
    EEPROM.put(ADDR_INIT, INIT_KEY);
    EEPROM.put(ADDR_YRKOST, 50);  // значение яркости по умолчанию -> Default brightness value
    EEPROM.put(ADDR_SVET, 1);  // состояние подвсетки по умолчанию -> Default backlight state
    EEPROM.put(ADDR_TEMP, 1);  // default temperature state
  }

  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  pinMode(SW4, INPUT);
  pinMode(led_pwm, OUTPUT);  // подсветка ламп -> Lamp backlight

  /*  Назначение кнопок:
      SW4 --- set - при однократном нажатии активируется режим корректировки минут(1), далее нажимая set - часы(2), день(3), месяц(4) и год(5)
      SW3 --- +/sec - при нажатии и удержании кнопки должны отображаться секунды "S_31"
      SW2 --- -/day - при нажатии и удержании кнопки должна отображаться дата
      SW1 --- light - при однократном нажатии меню включения/отключения подсветки, далее меню настройки яркости
    Button assignment:
    SW4 --- set - Pressing once activates the minute adjustment mode (1), then pressing set activates the hour (2), day (3), month (4), and year (5)
    SW3 --- +/sec - Pressing and holding the button should display the seconds "S_31"
    SW2 --- -/day - Pressing and holding the button should display the date
    SW1 --- light - Pressing once activates the backlight on/off menu, then the brightness adjustment menu
  */
 
 if (! rtc.begin()) {
    Serial.println("Couldn't find rtc");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("rtc lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the rtc to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the rtc with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the rtc to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the rtc with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

 DateTime now = rtc.now();
// sec = now.second();
 year  = now.year() - 2000;
 month = now.month();
 day = now.day();
//zis = now.dayOfTheWeek();
hour = now.hour();
minute = now.minute(); 
tt = rtc.getTemperature();
  
  EEPROM.get(ADDR_YRKOST, brightness);  // Чтение значения яркости подсветки -> Reading the backlight brightness value
  EEPROM.get(ADDR_SVET, svet_state);  // Чтение значения состояния подсветки -> Reading the backlight status value
  EEPROM.get(ADDR_TEMP, showt);  // Reading showing temperature status
svet_state = svet_state%2;  
menu_svet = 0;
 svet = brightness * 2.55;
  if (svet_state == 1) 
  {
    analogWrite(led_pwm, svet);
  } 
  else 
  {
    digitalWrite(led_pwm, LOW);
  }
}

void loop() {
  // Опрос времени -> Querying the time
  if (millis() - timer >= PERIOD) 
  {
    dps=1;
    if (dp != dps)
      {
      dp=1;
      } 
      else 
      {
        dp=0;
      }
    DateTime now = rtc.now();
    hh = now.hour();  // Часы -> hours
    mm = now.minute();  // Минуты -> minutes
    ss = now.second();  // Секунды -> seconds
    tt = (float)(rtc.getTemperature()+0.5); 
    Serial.print(hh/10);  
    Serial.print(hh%10);  
    Serial.print(":");
    Serial.print(mm/10);  
    Serial.print(mm%10);  
    Serial.print(":");
    Serial.print(ss/10);  
    Serial.println(ss%10);
    Serial.print("Temperature: ");
    Serial.print(tt);
    Serial.println("C");        
    do 
    {
      timer += PERIOD;
      if (timer < PERIOD) break;  // переполнение uint32_t
    }
    while (timer < millis() - PERIOD); // защита от пропуска шага
  } 
  else
    {
    dps=0;
    }
  
   if (analogRead(SW4) > 500 && menu_svet == 0) 
   {  // Вход в меню корректировки -> Enter the adjustment menu
    menu++;
    delay(200);
    if (menu > 6 && save_t == 1) 
      {
      menu = 0;w = 1;
      }   // Выход с изменением настроек времени -> Exits with the time settings changed
    if (menu > 6 && save_t == 0) 
      {
      menu = 0;
      w = 0;
      };
      } // Выход без изменения настроек времени -> Exits without changing the time settings
        
  // Вход в меню настройки подсветки и выход с сохранением в ЭСППЗУ
  // Enters the backlight adjustment menu and exits, saving the settings to EEPROM
  
  if (analogRead(SW1) > 500 && menu == 0) 
  {
    Serial.print("SW1 > menu = ");
    menu_svet++;
    delay(500);
    Serial.println(menu_svet);
    if (menu_svet > 3)  // was 2
    {
      menu_svet = 0;
      Serial.println("menu = 0"); 
      w3 = 1;
    };
   } 
   
  ///////////////////////////// Время -. Time /////////////////////////////
  if (menu == 0 && w2 == 0 && menu_svet == 0) 
  {
    if (((showt%2 == 1) and (ss < showts)) or (showt%2 == 0))
    {
    times = hh*100+mm;
    a[0] = times / 1000;
    a[1] = times / 100 % 10;
    if (dp == 1) 
    {  // Мигающее тире разделителяющее минуты и секунды -> Blinking dash separating minutes and seconds
     a[2] = 18;
    } 
   else 
    {
     a[2] = 10;
    }
     a[3] = times / 10 % 10;
     a[4] = times % 10 % 10;
  //   Serial.println(times);
    }
    else
    if ((showt%2 == 1) and (ss >= showts))
    {
    a[0] = 10;    // blank
    a[1] = tt/10;
    a[2] = tt%10;
    a[3] = 19;   // degree
    a[4] = 20;   // C   
 //   Serial.println("tt");  
    }
  }
  w2 = 0;

  ////////////////// вывод секунд - нажать и удерживать кнопку +/sec //////
  // Displays seconds - press and hold the +/sec button //
  if (analogRead(SW3) > 500 && menu == 0 && menu_svet == 0) 
  {
    w2 = 1; 
    times = ss;
    a[0] = 11;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;
  }

  ////////////////////////// Коррекция минут -> Adjusting minutes//////////////////////////
  if (menu == 1) 
    {
    if (analogRead(SW3) > 500) 
    {
      minute++;
      delay(300);
      if (minute > 59) 
      {
        minute = 0;
      }
     }
    if (analogRead(SW2) > 500)
    {
      minute--;
      delay(300);
      if (minute < 0) 
      {
        minute = 0;
      }
      }
    times = minute;
    a[0] = 1;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;
    }

  /////////////////////////// Коррекция часов  -> Adjusts hours ///////////////////////////
  if (menu == 2) 
  {
    if (analogRead(SW3) > 500) 
    {
      hour++;
      delay(300);
      if (hour > 23) 
      {
        hour = 0;
      }
    }
    if (analogRead(SW2) > 500) 
    {
      hour--;
      delay(300);
      if (hour < 0)
      {
        hour = 0;
      }
     }
    times = hour;
    a[0] = 2;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;}

  //////////////////////////// Коррекция даты -> Date correction //////////////////////////////
  if (menu == 3) {
    if (analogRead(SW3) > 500) 
    {
      day++;
      delay(300);
      if (day > 31) 
      {
        day = 1; // original was 0 (niq_ro added correction)
      }
     }
    if (analogRead(SW2) > 500) 
    {
      day--;
    delay(300);
    if (day < 0) 
    {
      day = 1; // original was 0 (niq_ro added correction)
    }
    }  
    times = day;
    a[0] = 3;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;}

  ////////////////////////// Коррекция месяца -> Month Correction /////////////////
  if (menu == 4) 
  {
    if (analogRead(SW3) > 500)
    {
      month++;
      delay(300);
      if (month > 12) 
      {
        month = 1;
      }
     }
    if (analogRead(SW2) > 500) 
    {
      month--;
      delay(300);
      if (month < 1) 
      {
        month = 1;
      }
      }
    times = month;
    a[0] = 4;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;
    }

  ///////////////////////////// Коррекция года -> Year Adjustment ////////////////////////
  if (menu == 5) 
  {
    if (analogRead(SW3) > 500 ) 
    {
      year++;
      delay(300);
      if (year > 99) 
      {
        year = 26;
      }
    }
    if (analogRead(SW2) > 500) 
    {
      year--;
      delay(300);
      if (year < 26) 
      {
        year = 26;
      }
    }
    times = year;
    a[0] = 5;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10 % 10;
    a[4] = times % 10;}

  ///////Запрос на сохранение изменений корректировки времени -> Request to save time adjustment changes ////////
  if (menu == 6) 
  {
    if (analogRead(SW3) > 500) 
    {
      save_t++;
      delay(300);
      if (save_t > 1) 
      {
        save_t = 0;
      }
     }
    if (analogRead(SW2) > 500) 
    {
      save_t--;
      delay(300);
      if (save_t < 0) 
      {
        save_t = 1;
      }
    }
    if (save_t == 1) 
    {
      a[0] = 11;  // S
      a[1] = 17;  // A
      a[2] = 10;  
      a[3] = 10;
      a[4] = 16;  // y
      }
    if (save_t == 0) 
    {
      a[0] = 11;  // S
      a[1] = 17;  // A
      a[2] = 10;
      a[3] = 10;
      a[4] = 14;  // n
    }
    }

  if (w==1) 
  {  // Сохранение времени при флаге w==1 -> Saving time when w==1
    rtc.adjust(DateTime(2000+year, month, day, hour, minute, 59));
    w=0;
    delay(1000);
  }
  
  ///////////////////// Включение/выключение подсветки -> Turns the backlight on/off //////////////////
  if (menu_svet == 1) 
  {
    if (analogRead(SW3) > 500) 
    {
      Serial.println("SW3");
      svet_state++;
      delay(300);
      if (svet_state > 1) 
      {
        svet_state = 0;
      }
     }
    if (analogRead(SW2) > 500) 
    {
      Serial.println("SW2");
      svet_state--;
      delay(300);
      if (svet_state < 0) 
      {
        svet_state = 1;
      }
     }
    if (svet_state%2 == 1) 
    {
      svet_state = svet_state%2;
      a[0] = 15; // L;
      a[1] = 10;
      a[2] = 10;
      a[3] = 0;  // o
      a[4] = 14; // n
    }
    if (svet_state%2 == 0) 
    {
      svet_state = svet_state%2;
      a[0] = 15;  //L;
      a[1] = 10;
      a[2] = 0;   // o
      a[3] = 13;  // f
      a[4] = 13;  // f
    }
    }

  ////////////////////// Настройка яркости подсветки -> Adjusts the backlight brightness ////////////////
  if (menu_svet == 2) 
  {
    if (analogRead(SW3) > 500) 
    {
      Serial.println("Brightness +");
      brightness++;
      delay(200);
      if (brightness > 100) 
      {
        brightness = 100;
      }
      }
    if (analogRead(SW2) > 500) 
    {
      Serial.println("Brightness -");
      brightness--;
      delay(200);
      if (brightness < 0) 
      {
        brightness = 0;
      }
    }
    Serial.print("Brightness = ");
    Serial.println(brightness);
    times = brightness;
    a[0] = 15;
    a[1] = 10;
    if (times == 100) 
    {
      a[2] = 1;
    }
    else 
    {
      a[2] = 10;
    }
    if (times > 9) 
    {
      a[3] = times / 10 % 10;
    } 
    else 
    {
      a[3] = 10;
    }
    a[4] = times % 10;
  };

  svet = brightness * 2.55;
  if (svet_state == 1) 
  {
    analogWrite(led_pwm, svet);
  } 
  else 
  {
    digitalWrite(led_pwm, LOW);
  }

  ///////////////////// Show Temperature: on/off //////////////////
  if (menu_svet == 3) 
  {
    if (analogRead(SW3) > 500) 
    {
      Serial.println("SW3");
      showt++;
      delay(300);
      if (showt > 1) 
      {
        showt = 0;
      }
     }
    if (analogRead(SW2) > 500) 
    {
      Serial.println("SW2");
      showt--;
      delay(300);
      if (showt < 0) 
      {
        showt = 1;
      }
     }
    if (showt == 1) 
    {
      a[0] = 12; // t;
      a[1] = 10;  
      a[2] = 10;
      a[3] = 0;  // o
      a[4] = 14; // n
    }
    if (showt == 0) 
    {
      a[0] = 12;  //t;
      a[1] = 10;
      a[2] = 0;   // o
      a[3] = 13;  // f
      a[4] = 13;  // f
    }
    }

  if (w3 == 1) 
  {   // Сохранение настроек в ЭСППЗУ при флаге w3==1 -> Saving settings to EEPROM when the w3 flag is set to 1
    EEPROM.put(ADDR_YRKOST, brightness);
    EEPROM.put(ADDR_SVET, svet_state%2);
    EEPROM.put(ADDR_TEMP, showt);
    w3 = 0;
  }
  
}  // end loop

void to_Timer() 
{
  switch (i) 
  {
    case 0: 
      cl(); 
      segm = a[0];
      an = 0;
      anod();
      segment();
      break;
    case 1: 
      cl();
      segm = a[1];
      an = 1;
      anod();
      segment();
      break;
    case 2:
      cl();
      segm = a[2];
      an = 2;
      anod();
      segment();
      break;
    case 3: 
      cl(); 
      segm = a[3];
      an = 3;
      anod();
      segment();
      break;
    case 4:
      cl();
      segm = a[4];
      an = 4;
      anod();
      segment();
      break;
  } 
  i++;
  if (i > 4) 
  {
    i = 0;
  };
}

void segment() 
{
  switch (segm) 
  {
    case 0:  
      ch(SEG_A, 0);
      ch(SEG_B, 0);
      ch(SEG_C, 0);
      ch(SEG_D, 0);
      ch(SEG_E, 0);
      ch(SEG_F, 0);
      ch(SEG_G, 1);
      break; // 0
    case 1:  
      ch(SEG_A, 1);
      ch(SEG_B, 0);
      ch(SEG_C, 0);
      ch(SEG_D, 1);
      ch(SEG_E, 1);
      ch(SEG_F, 1);
      ch(SEG_G, 1);
      break; // 1
    case 2: 
      ch(SEG_A, 0);
      ch(SEG_B, 0);
      ch(SEG_C, 1);
      ch(SEG_D, 0);
      ch(SEG_E, 0);
      ch(SEG_F, 1);
      ch(SEG_G, 0);
      break; // 2
    case 3:  
      ch(SEG_A, 0);
      ch(SEG_B, 0);
      ch(SEG_C, 0);
      ch(SEG_D, 0);
      ch(SEG_E, 1);
      ch(SEG_F, 1);
      ch(SEG_G, 0);
      break; // 3
    case 4:  
      ch(SEG_A, 1);
      ch(SEG_B, 0);
      ch(SEG_C, 0);
      ch(SEG_D, 1);
      ch(SEG_E, 1);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // 4
    case 5:  
      ch(SEG_A, 0);
      ch(SEG_B, 1);
      ch(SEG_C, 0);
      ch(SEG_D, 0);
      ch(SEG_E, 1);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // 5
    case 6:  
      ch(SEG_A, 0);
      ch(SEG_B, 1);
      ch(SEG_C, 0);
      ch(SEG_D, 0);
      ch(SEG_E, 0);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // 6
    case 7:  
      ch(SEG_A, 0);
      ch(SEG_B, 0);
      ch(SEG_C, 0);
      ch(SEG_D, 1);
      ch(SEG_E, 1);
      ch(SEG_F, 1);
      ch(SEG_G, 1);
      break; // 7
    case 8:  
      ch(SEG_A, 0);
      ch(SEG_B, 0);
      ch(SEG_C, 0);
      ch(SEG_D, 0);
      ch(SEG_E, 0);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // 8
    case 9:  
      ch(SEG_A, 0);
      ch(SEG_B, 0);
      ch(SEG_C, 0);
      ch(SEG_D, 0);
      ch(SEG_E, 1);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // 9
    case 10: 
      ch(SEG_A, 1); 
      ch(SEG_B, 1); 
      ch(SEG_C, 1); 
      ch(SEG_D, 1);
      ch(SEG_E, 1);
      ch(SEG_F, 1);
      ch(SEG_G, 1);
      break; // ПУСТО -> BLANK
    case 11: 
      ch(SEG_A, 0);
      ch(SEG_B, 1);
      ch(SEG_C, 0);
      ch(SEG_D, 0);
      ch(SEG_E, 1);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // S
    case 12: 
      ch(SEG_A, 1);
      ch(SEG_B, 1);
      ch(SEG_C, 1);
      ch(SEG_D, 0);
      ch(SEG_E, 0);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // t
    case 13: 
      ch(SEG_A, 0);
      ch(SEG_B, 1);
      ch(SEG_C, 1);
      ch(SEG_D, 1);
      ch(SEG_E, 0);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // F
    case 14:
      ch(SEG_A, 1); 
      ch(SEG_B, 1);
      ch(SEG_C, 0);
      ch(SEG_D, 1);
      ch(SEG_E, 0); 
      ch(SEG_F, 1); 
      ch(SEG_G, 0);
      break; // n
    case 15: 
      ch(SEG_A, 1);
      ch(SEG_B, 1),
      ch(SEG_C, 1);
      ch(SEG_D, 0);
      ch(SEG_E, 0);
      ch(SEG_F, 0);
      ch(SEG_G, 1);
      break; // L
    case 16: 
      ch(SEG_A, 1); 
      ch(SEG_B, 0);
      ch(SEG_C, 0);
      ch(SEG_D, 0);
      ch(SEG_E, 1);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // y
    case 17: 
      ch(SEG_A, 0);
      ch(SEG_B, 0); 
      ch(SEG_C, 0);
      ch(SEG_D, 1); 
      ch(SEG_E, 0); 
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // A
    case 18: 
      ch(SEG_A, 1);
      ch(SEG_B, 1);
      ch(SEG_C, 1); 
      ch(SEG_D, 1); 
      ch(SEG_E, 1);
      ch(SEG_F, 1);
      ch(SEG_G, 0);
      break; // -
      case 19: 
      ch(SEG_A, 0);
      ch(SEG_B, 0);
      ch(SEG_C, 1); 
      ch(SEG_D, 1); 
      ch(SEG_E, 1);
      ch(SEG_F, 0);
      ch(SEG_G, 0);
      break; // degree - added by niq_ro 
      case 20: 
      ch(SEG_A, 0);
      ch(SEG_B, 1);
      ch(SEG_C, 1); 
      ch(SEG_D, 0); 
      ch(SEG_E, 0);
      ch(SEG_F, 0);
      ch(SEG_G, 1);
      break; // C - added by niq_ro    
  }
}
void anod() 
{
  switch (an) 
  {
    case 0: 
      ch(grids_HL1, 0);
      ch(grids_HL2, 1);
      ch(grids_HL3, 1);
      ch(grids_HL4, 1);
      ch(grids_HL5, 1);
      break;
    case 1:
      ch(grids_HL1, 1);
      ch(grids_HL2, 0);
      ch(grids_HL3, 1);
      ch(grids_HL4, 1);
      ch(grids_HL5, 1);
      break;
    case 2: 
      ch(grids_HL1, 1);
      ch(grids_HL2, 1);
      ch(grids_HL3, 0);
      ch(grids_HL4, 1);
      ch(grids_HL5, 1);
      break;
    case 3:
      ch(grids_HL1, 1);
      ch(grids_HL2, 1);
      ch(grids_HL3, 1);
      ch(grids_HL4, 0);
      ch(grids_HL5, 1);
      break;
    case 4:
      ch(grids_HL1, 1);
      ch(grids_HL2, 1);
      ch(grids_HL3, 1);
      ch(grids_HL4, 1);
      ch(grids_HL5, 0);
      break;
  }
}

void cl() 
  {
  segm = 10;
  an = 0;
    segment();
    anod();
  an = 1;
    segment();
    anod();
  an = 2;
    segment();
     anod();
  an = 3;
    segment();
    anod();
  an = 4;
    segment();
     anod();
  }

void ch(int pin, int logic) 
  {
  digitalWrite(pin, logic);
  }
