#include <Arduino.h>
#include <Gyver433.h>
#include <avr/sleep.h>
#include "Led/LedFader.h"

Gyver433_RX<10, 4> rx;

const int FUTURE_INTERUPT_PIN = 2;
bool sleepLOW = false;
unsigned long lastPowerOff = 0;
const long TIME_BEFORE_SLEEP = 300000;
unsigned long pause = 0;

LedFader RedLed(9, 1, 0, 255);   // Cветодиод красный
LedFader YellowLed1(5, 5, 0, 255);   // Cветодиод красный
LedFader YellowLed2(6, 5, 0, 255);   // Cветодиод жёлтый
unsigned long currentTime = 0;

unsigned long FireON_Message = 0;
unsigned long FireOFF_Message = 0;
unsigned int FIRE_CD = 35000; // константа времени на случай,, если отвалится радиосообщение
unsigned int FIRE_OFF = 4000; // Время для затухания пламени

enum DeviceState {
    STATE_IDLE,              // Розжиг
    STATE_FIRE_ON,              // Полное пламя
    STATE_FIRE_OFF,      // Затухание пламени
    STATE_WAIT_IDLE
};

DeviceState currentState = STATE_IDLE;
void initFire();
void fullFire();
void shutOff();
void waitIdle();

void SleepHandler(){
  if(digitalRead(FUTURE_INTERUPT_PIN) == LOW){
    sleepLOW = true;
    lastPowerOff = millis();
  } else {
    sleepLOW = false;
    Serial.println("AWAKE");
  }
}

void goToSleep(){
    Serial.println("SLEEP");
    YellowLed1.setBrightness(0);
    YellowLed2.setBrightness(0);
    RedLed.setBrightness(0);

    delay(1000);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Выбираем глубокий сон
    sleep_enable(); // Разрешаем сон
    sei();  // Разрешаем прерывания
    sleep_cpu(); // Переводим микроконтроллер в сон (здесь выполнение останавливается)
    
    // --- Процесс пробуждения начинается здесь после нажатия кнопки ---
    sleep_disable();
}

void setup() {
  Serial.begin(9600);
  // pinMode(FUTURE_INTERUPT_PIN, INPUT_PULLUP);

  attachInterrupt(0, SleepHandler, CHANGE);

  RedLed.initLed();
  YellowLed1.initLed();
  YellowLed2.initLed();
}


void loop() {
  if (rx.tick()) {
    rx.buffer[rx.size] = '\0'; 
    String result = String((char*)rx.buffer);


    if(result == "ST"){
      FireON_Message = millis();
      currentState = STATE_FIRE_ON;
      Serial.println("X");
      
    } else if (result == "OF"){
      FireOFF_Message = millis();
      currentState = STATE_FIRE_OFF;
      Serial.println("Y");

    } else if (result == "ID"){
      currentState = STATE_IDLE;
      Serial.println("Z");
    }
    
  }

  if(digitalRead(FUTURE_INTERUPT_PIN) == LOW && millis() - lastPowerOff > TIME_BEFORE_SLEEP){
    goToSleep();
  }

  // if(millis() - pause > 1000){
  //   Serial.println(pause);
  //   pause = millis();
  // }
  

  switch (currentState) {
        case STATE_IDLE:
            initFire();
            break;

        case STATE_FIRE_ON:
            fullFire();
            break;

        case STATE_FIRE_OFF:
            shutOff();
            break;

        case STATE_WAIT_IDLE:
            waitIdle();
    }
}

void initFire(){
  currentTime = millis();
  if (currentTime - RedLed._fadeIntervalTime >= 100) { 
        RedLed._fadeIntervalTime = currentTime;
        RedLed.startFading(15, 10);
  }
}

void fullFire(){
  currentTime = millis();
  if(currentTime - FireON_Message < FIRE_CD){
    if (currentTime - RedLed._fadeIntervalTime >= 100) { 
            RedLed._fadeIntervalTime = currentTime;
            RedLed.startFading(200, 40);
      }
      if (currentTime - YellowLed1._fadeIntervalTime >= 90) { 
            YellowLed1._fadeIntervalTime = currentTime;
            YellowLed1.startFading(200, 15);
      }
      if (currentTime - YellowLed2._fadeIntervalTime >= 80) { 
            YellowLed2._fadeIntervalTime = currentTime;
            YellowLed2.startFading(200, 15);
      }
  } else if(currentState == STATE_FIRE_ON){
    Serial.println('Y1');
    FireOFF_Message = millis();
    currentState = STATE_FIRE_OFF;
  }
}

void shutOff(){
  currentTime = millis();
  if(currentTime - FireOFF_Message < FIRE_OFF){
    if (currentTime - RedLed._fadeIntervalTime >= 30) { 
            RedLed._fadeIntervalTime = currentTime;
            RedLed.fadeToMin();
      }
      if (currentTime - YellowLed1._fadeIntervalTime >= 30) { 
            YellowLed1._fadeIntervalTime = currentTime;
            YellowLed1.fadeToMin();
      }
      if (currentTime - YellowLed2._fadeIntervalTime >= 30) { 
            YellowLed2._fadeIntervalTime = currentTime;
            YellowLed2.fadeToMin();
      }
  } else {
    YellowLed1.setBrightness(0);
    YellowLed2.setBrightness(0);
    RedLed.setBrightness(0);
    currentState = STATE_WAIT_IDLE;
  }
  
  
}

void waitIdle(){
  currentTime = millis();
  if(currentTime - FireOFF_Message > FIRE_CD + FIRE_OFF && currentState == STATE_FIRE_OFF){
      Serial.println('Z1');
      currentState = STATE_IDLE;
  }
}