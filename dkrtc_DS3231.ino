/* 
 http://forum.rcl-radio.ru/viewtopic.php?id=87&p=5
 http://rcl-radio.ru/?p=67405
 https://oshwlab.com/allexsemenow/iv-6-clock_copy
 v.0 - changed for DS3231 by niq_ro (Nicu FLORICA)
*/

#define ADDR_INIT 6    // Адрес для проверки
#define INIT_KEY 15    // Ключ проверки на первый запуск
#define ADDR_YRKOST 0  // Адрес хранения значения яркости
#define ADDR_SVET 2    // Адрес хранения состояния подсветки
#define PERIOD 1000

#include "MsTimer2.h"  // // http://rcl-radio.ru/wp-content/uploads/2018/11/MsTimer2.zip 
#include <EEPROM.h>
#include <Wire.h>
#include <iarduino_RTC.h>  // https://github.com/tremaru/iarduino_RTC
//iarduino_RTC watch(RTC_DS1302, A4, A3, A5);  // RST, CLK, DAT
iarduino_RTC watch(RTC_DS3231);    
int an, segm, times, i, ss, mm, hh;
int dp;
byte a[5];  // масив содержащий выводимую на индикаторы информацию
int menu, year, month, day, hour, minute, second, weekday, w, w2, save_t;
int menu_svet, svet_state, brightness, svet, w3;
uint32_t timer = 0;
int led_pwm = 10;
int dps;
///////////// Управляющие сетки ВЛИ/////////////////////////
int grids_HL1 = 4;
int grids_HL2 = 3;
int grids_HL3 = 2;
int grids_HL4 = 13;
int grids_HL5 = A0;
////////////////////////// Сегменты ВЛИ////////////////////////
int SEG_A = 7;
int SEG_B = 5;
int SEG_C = 6;
int SEG_D = 12;
int SEG_E = 11;
int SEG_F = 9;
int SEG_G = 8;
//////////////////////////////////// Кнопки //////////////////////
int SW1 = A1;
int SW2 = A2;
int SW3 = A6;
int SW4 = A7;

void setup() {
  watch.begin();  // Инициируем работу с модулем.
  MsTimer2::set(2, to_Timer);
  MsTimer2::start();
  Serial.begin(9600);
  //Serial.end();  // Отключаем последовательный порт

  if (EEPROM.read(ADDR_INIT) != INIT_KEY) {  // проверка на первый запуск
    EEPROM.put(ADDR_INIT, INIT_KEY);
    EEPROM.put(ADDR_YRKOST, 50);  // значение яркости по умолчанию
    EEPROM.put(ADDR_SVET, 1);  // состояние подвсетки по умолчанию
  }

  //////////////////////// Настройка выводов микроконтроллера////////////////
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
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  pinMode(SW4, INPUT);
  pinMode(led_pwm, OUTPUT);  // подсветка ламп

  /*  Назначение кнопок:
      SW4 --- set - при однократном нажатии активируется режим корректировки минут(1), далее нажимая set - часы(2), день(3), месяц(4) и год(5)
      SW3 --- +/sec - при нажатии и удержании кнопки должны отображаться секунды "S_31"
      SW2 --- -/day - при нажатии и удержании кнопки должна отображаться дата
      SW1 --- light - при однократном нажатии меню включения/отключения подсветки, далее меню настройки яркости
  */
  watch.gettime();
  year = watch.year;  // текущее время перенос в переменные коррекции времени
  month = watch.month;
  day = watch.day;
  hour = watch.Hours;
  minute = watch.minutes;

  EEPROM.get(ADDR_YRKOST, brightness);  // Чтение значения яркости подсветки
  EEPROM.get(ADDR_SVET, svet_state);  // Чтение значения состояния подсветки
}
void loop() {
  // Опрос времени
  if (millis() - timer >= PERIOD) {
    dps=1;
    if (dp != dps){
      dp=1;} else {dp=0;}
    watch.gettime();
    hh = watch.Hours;  // Часы - hours
    mm = watch.minutes;  // Минуты - minutes
    ss = watch.seconds;  // Секунды - seconds
    Serial.println(watch.gettime("d-m-Y, H:i:s, D"));  // Выводим время. - display the time
    do {
      timer += PERIOD;
      if (timer < PERIOD) break;  // переполнение uint32_t - overflow 4,294,967,295
    } while (timer < millis() - PERIOD); // защита от пропуска шага
  } else {dps=0;}
  
   if (analogRead(SW4) > 500 && menu_svet == 0) {  // Вход в меню корректировки - Enter the adjustment menu
    menu++; delay(200); if (menu > 6 && save_t == 1) {menu = 0;w = 1;}  // Выход с изменением настроек времени - Exit with the time settings changed
                        if (menu > 6 && save_t == 0) {menu = 0;w = 0;};}// Выход без изменения настроек времени - Exit without changing time settings
  // Вход в меню настройки подсветки и выход с сохранением в ЭСППЗУ
  if (analogRead(SW1) > 500 && menu == 0) {menu_svet++;delay(200); if (menu_svet > 2) {menu_svet = 0;w3 = 1;};} 
  ///////////////////////////// Время/////////////////////////////
  if (menu == 0 && w2 == 0 && menu_svet == 0) {times = hh*100+mm;
    a[0] = times / 1000;
    a[1] = times / 100 % 10;
    if (dp == 1) {  // Мигающее тире разделителяющее минуты и секунды
      a[2] = 18;
    } else {
      a[2] = 10;}
    a[3] = times / 10 % 10;
    a[4] = times % 10 % 10;
  }
  w2 = 0;

  ////////////////// вывод секунд - нажать и удерживать кнопку +/sec //////
  if (analogRead(SW3) > 500 && menu == 0 && menu_svet == 0) {
    w2 = 1; times = ss;
    a[0] = 11;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;
  }

  ////////////////////////// Коррекция минут//////////////////////////
  if (menu == 1) {
    if (analogRead(SW3) > 500 ) {minute++;delay(300);if (minute > 59) {minute = 0;}}
    if (analogRead(SW2) > 500 ) {minute--;delay(300);if (minute < 0) {minute = 0;}}
    times = minute;
    a[0] = 1;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;}

  /////////////////////////// Коррекция часов///////////////////////////
  if (menu == 2) {
    if (analogRead(SW3) > 500 ) {hour++;delay(300);if (hour > 23) {hour = 0;}}
    if (analogRead(SW2) > 500 ) {hour--;delay(300);if (hour < 0) {hour = 0;}}
    times = hour;
    a[0] = 2;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;}

  //////////////////////////// Коррекция даты//////////////////////////////
  if (menu == 3) {
    if (analogRead(SW3) > 500 ) {day++;delay(300);if (day > 31) {day = 0;}}
    if (analogRead(SW2) > 500 ) {day--;delay(300);if (day < 0) {day = 0;}}
    times = day;
    a[0] = 3;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;}

  ////////////////////////// Коррекция месяца /////////////////
  if (menu == 4) {
    if (analogRead(SW3) > 500 ) {month++;delay(300);if (month > 12) {month = 1;}}
    if (analogRead(SW2) > 500 ) {month--;delay(300);if (month < 1) {month = 1;}}
    times = month;
    a[0] = 4;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10;
    a[4] = times % 10;}

  ///////////////////////////// Коррекция года ////////////////////////
  if (menu == 5) {
    if (analogRead(SW3) > 500 ) {year++;delay(300);if (year > 99) {year = 19;}}
    if (analogRead(SW2) > 500 ) {year--;delay(300);if (year < 19) {year = 19;}}
    times = year;
    a[0] = 5;
    a[1] = 10;
    a[2] = 10;
    a[3] = times / 10 % 10;
    a[4] = times % 10;}

  ///////Запрос на сохранение изменений корректировки времени////////
  if (menu == 6) {
    if (analogRead(SW3) > 500 ) {save_t++;delay(300);if (save_t > 1) {save_t = 0;}}
    if (analogRead(SW2) > 500 ) {save_t--;delay(300);if (save_t < 0) {save_t = 1;}}
    if (save_t == 1) {
      a[0] = 11;
      a[1] = 17;
      a[2] = 10;
      a[3] = 10;
      a[4] = 16;}
    if (save_t == 0) {
      a[0] = 11;
      a[1] = 17;
      a[2] = 10;
      a[3] = 10;
      a[4] = 14;}}

  if (w==1) {  // Сохранение времени при флаге w==1
    watch.settime(59,minute,hour,day,month,year,weekday);  // 59  сек, 52 мин, 22 часа, 2, мая, 2021 года, воскресенье
    w=0;
  }
  ///////////////////// Включение/выключение подсветки//////////////////
  if (menu_svet == 1) {
    if (analogRead(SW3) > 500 ) {svet_state++;delay(300);if (svet_state > 1) {svet_state = 0;}}
    if (analogRead(SW2) > 500 ) {svet_state--;delay(300);if (svet_state < 0) {svet_state = 1;}}
    if (svet_state == 1) {
      a[0] = 10;
      a[1] = 10;
      a[2] = 10;
      a[3] = 0;
      a[4] = 14;}
    if (svet_state == 0) {
      a[0] = 10;
      a[1] = 10;
      a[2] = 0;
      a[3] = 13;
      a[4] = 13;}}

  ////////////////////// Настройка яркости подсветки ////////////////
  if (menu_svet == 2) {
    if (analogRead(SW3) > 500 ) {brightness++;delay(200);if (brightness > 100) {brightness = 100;}}
    if (analogRead(SW2) > 500 ) {brightness--;delay(200);if (brightness < 0) {brightness = 0;}}
    times = brightness;
    a[0] = 15;
    a[1] = 10;
    if (times == 100) {
      a[2] = 1;
    } else {
      a[2] = 10;
    }
    if (times > 9) {
      a[3] = times / 10 % 10;
    } else {
      a[3] = 10;
    }
    a[4] = times % 10;
  };

  svet = brightness * 2.55;
  if (svet_state == 1) {
    analogWrite(led_pwm, svet);
  } else {
    digitalWrite(led_pwm, LOW);
  }

  if (w3 == 1) {  // Сохранение настроек в ЭСППЗУ при флаге w3==1
    EEPROM.put(ADDR_YRKOST, brightness); EEPROM.put(ADDR_SVET, svet_state);
    w3 = 0;
  }
}  // loop

void to_Timer() {
  switch (i) {
    case 0: cl(); segm = a[0]; an = 0; anod(); segment(); break;
    case 1: cl(); segm = a[1]; an = 1; anod(); segment(); break;
    case 2: cl(); segm = a[2]; an = 2; anod(); segment(); break;
    case 3: cl(); segm = a[3]; an = 3; anod(); segment(); break;
    case 4: cl(); segm = a[4]; an = 4; anod(); segment(); break;
  } i++; if (i > 4) {i = 0;};}

void segment() {
  switch (segm) {

    case 0:  ch(SEG_A, 0); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 0); ch(SEG_E, 0); ch(SEG_F, 0); ch(SEG_G, 1); break; // 0
    case 1:  ch(SEG_A, 1); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 1); ch(SEG_E, 1); ch(SEG_F, 1); ch(SEG_G, 1); break; // 1
    case 2:  ch(SEG_A, 0); ch(SEG_B, 0); ch(SEG_C, 1); ch(SEG_D, 0); ch(SEG_E, 0); ch(SEG_F, 1); ch(SEG_G, 0); break; // 2
    case 3:  ch(SEG_A, 0); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 0); ch(SEG_E, 1); ch(SEG_F, 1); ch(SEG_G, 0); break; // 3
    case 4:  ch(SEG_A, 1); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 1); ch(SEG_E, 1); ch(SEG_F, 0); ch(SEG_G, 0); break; // 4
    case 5:  ch(SEG_A, 0); ch(SEG_B, 1); ch(SEG_C, 0); ch(SEG_D, 0); ch(SEG_E, 1); ch(SEG_F, 0); ch(SEG_G, 0); break; // 5
    case 6:  ch(SEG_A, 0); ch(SEG_B, 1); ch(SEG_C, 0); ch(SEG_D, 0); ch(SEG_E, 0); ch(SEG_F, 0); ch(SEG_G, 0); break; // 6
    case 7:  ch(SEG_A, 0); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 1); ch(SEG_E, 1); ch(SEG_F, 1); ch(SEG_G, 1); break; // 7
    case 8:  ch(SEG_A, 0); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 0); ch(SEG_E, 0); ch(SEG_F, 0); ch(SEG_G, 0); break; // 8
    case 9:  ch(SEG_A, 0); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 0); ch(SEG_E, 1); ch(SEG_F, 0); ch(SEG_G, 0); break; // 9
    case 10: ch(SEG_A, 1); ch(SEG_B, 1); ch(SEG_C, 1); ch(SEG_D, 1); ch(SEG_E, 1); ch(SEG_F, 1); ch(SEG_G, 1); break; // ПУСТО
    case 11: ch(SEG_A, 0); ch(SEG_B, 1); ch(SEG_C, 0); ch(SEG_D, 0); ch(SEG_E, 1); ch(SEG_F, 0); ch(SEG_G, 0); break; // S
    case 12: ch(SEG_A, 1); ch(SEG_B, 1); ch(SEG_C, 1); ch(SEG_D, 0); ch(SEG_E, 0); ch(SEG_F, 0); ch(SEG_G, 0); break; // t
    case 13: ch(SEG_A, 0); ch(SEG_B, 1); ch(SEG_C, 1); ch(SEG_D, 1); ch(SEG_E, 0); ch(SEG_F, 0); ch(SEG_G, 0); break; // F
    case 14: ch(SEG_A, 1); ch(SEG_B, 1); ch(SEG_C, 0); ch(SEG_D, 1); ch(SEG_E, 0); ch(SEG_F, 1); ch(SEG_G, 0); break; // n
    case 15: ch(SEG_A, 1); ch(SEG_B, 1), ch(SEG_C, 1); ch(SEG_D, 0); ch(SEG_E, 0); ch(SEG_F, 0); ch(SEG_G, 1); break; // L
    case 16: ch(SEG_A, 1); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 0); ch(SEG_E, 1); ch(SEG_F, 0); ch(SEG_G, 0); break; // y
    case 17: ch(SEG_A, 0); ch(SEG_B, 0); ch(SEG_C, 0); ch(SEG_D, 1); ch(SEG_E, 0); ch(SEG_F, 0); ch(SEG_G, 0); break; // A
    case 18: ch(SEG_A, 1); ch(SEG_B, 1); ch(SEG_C, 1); ch(SEG_D, 1); ch(SEG_E, 1); ch(SEG_F, 1); ch(SEG_G, 0); break; // -
  }
}
void anod() {
  switch (an) {
    case 0: ch(grids_HL1, 0); ch(grids_HL2, 1); ch(grids_HL3, 1); ch(grids_HL4, 1); ch(grids_HL5, 1); break;
    case 1: ch(grids_HL1, 1); ch(grids_HL2, 0); ch(grids_HL3, 1); ch(grids_HL4, 1); ch(grids_HL5, 1); break;
    case 2: ch(grids_HL1, 1); ch(grids_HL2, 1); ch(grids_HL3, 0); ch(grids_HL4, 1); ch(grids_HL5, 1); break;
    case 3: ch(grids_HL1, 1); ch(grids_HL2, 1); ch(grids_HL3, 1); ch(grids_HL4, 0); ch(grids_HL5, 1); break;
    case 4: ch(grids_HL1, 1); ch(grids_HL2, 1); ch(grids_HL3, 1); ch(grids_HL4, 1); ch(grids_HL5, 0); break;
  }
}

void cl() {
  segm = 10;
  an = 0; segment(); anod();
  an = 1; segment(); anod();
  an = 2; segment(); anod();
  an = 3; segment(); anod();
  an = 4; segment(); anod();
}

void ch(int pin, int logic) {digitalWrite(pin, logic);}
