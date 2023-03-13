#include <GyverOLED.h>
// Для энкодера
#include "GyverEncoder.h"
#include "TimerOne.h"
// шаговый двигатель
#include "GyverStepper2.h"





/*             todo сделать более красиво то что в этом блоке
---- encoder
 clk D2
 dt  D3
 sw  D4
 +   5V
 GND GND
 
---- temp 
 r   A0
 
---- display
 vdd 5V
 gnd GND
 sck A5
 sda A4
 
---- stepper driver 
 step 5
 dir  6
 en   7
 */

// ---------------- Основные параметры плавильни ---------------- 

// Настроенная температура  от TEMP_MIN, до TEMP_MAX  
#define TEMP_MIN 80
#define TEMP_MAX 500
static uint16_t enteredTemp = 200;

// диапазон допустимых колебаний температуры при плавке
// например 4 - это +-4 градуса
#define TEMP_VARIATE 4

// Температура с датчика
static uint16_t sensorTemp = 222;

// Текущая скорость мотора
#define SPEED_COEF 2.241758 // коэффициэнт пресчета (столько мм в минуту дает однаединица скорости)
#define SPEED_MIN 20
#define SPEED_MAX 2000
int enteredMotorSpeed = 998;

// Режим настройки 0 - ничего, 1 - настройка температуры
static uint8_t settingMode = 0;


// ---------------- Подключенный обвес ---------------- 
// транзистор нагрева
#define HOT_PIN 11

// датчик обрыва
#define ROLLOUT_PIN 8


// -- Параметры энкодера --
#define CLK 2
#define DT 3
#define SW 4
Encoder enc1(CLK, DT, SW);

// -- Параметры дисплея --
// переменная дисплей
GyverOLED<SSH1106_128x64> oled;

// -- Термистор --
int R1 = 10000;
float logR2, R2;
//steinhart-hart coeficients for thermistor
float c1 = 0.8438162826e-03, c2 = 2.059601750e-04, c3 = 0.8615484887e-07;

uint16_t Thermistor(float Volts) {
  R2 = R1 * (1023.0 / (float)Volts - 1.0); //calculate resistance on thermistor

  logR2 = log(R2);
  float T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)); // temperature in Kelvin  
  return (uint16_t) T - 273; //convert Kelvin to Celcius
}

// -- шаговый двигатель --
GStepper2<STEPPER2WIRE> stepper(2048, 5, 6, 7);// син, зел, бел  |  step, dir, en


// ---------------- Прерывания ---------------- 

// опрос энкодера и кнопок
bool lastButtonstate = false;
void timerIsr() {  // прерывание таймера

  // обработка энкодера
  enc1.tick();
  // изменяем значения при повороте энкодера
  // Настройка температуры
  if(settingMode == 1){
    if (enc1.isRight()) {enteredTemp = (enteredTemp < TEMP_MAX)?(enteredTemp + 1):(TEMP_MAX);}      
    if (enc1.isLeft()) {enteredTemp = (enteredTemp > TEMP_MIN)?(enteredTemp - 1):(TEMP_MIN);}
  }else 
  // Настройка скорости
  if(settingMode == 2){
    if (enc1.isRight()) {enteredMotorSpeed = (enteredMotorSpeed < SPEED_MAX)?(enteredMotorSpeed + 1):(SPEED_MAX);}      
    if (enc1.isLeft()) {enteredMotorSpeed = (enteredMotorSpeed > SPEED_MIN)?(enteredMotorSpeed - 1):(SPEED_MIN);}
  }
  // если было удержание + поворот
  //if (enc1.isRightH()) Serial.println("Right holded"); 
  //if (enc1.isLeftH()) Serial.println("Left holded");


  // Нажатие на кнопку
  // Если кнопка была отпущена и сейчас нажата 
  if(!lastButtonstate && !digitalRead(4)){
    lastButtonstate = true;
  } else
  // Если кнопка была нажата и сейчас отпущена 
  if(lastButtonstate && digitalRead(4)){
    lastButtonstate = false;  
    settingMode = (settingMode >= 2)?(0):(settingMode + 1);
  }


  // обработка шагового двигателя 
  // здесь происходит движение моторов, вызывать как можно чаще
  stepper.tick();


  // Обработка датчика обрыва
}


// ---------------- Основная программа ---------------- 
void setup() {
  // инициализация транзистора нагрева
  pinMode(HOT_PIN, OUTPUT);
  digitalWrite(HOT_PIN, 0);

  // инициализация датчика обрыва
  pinMode(ROLLOUT_PIN, INPUT);
  digitalWrite(ROLLOUT_PIN, 1);


  // инициализация энкодера
  enc1.setType(TYPE2);// тип энкодера TYPE2 двухшаговый
  // прерывание опроса энкодера и кнопок
  Timer1.initialize(1000);// каждые 1000 микросекунд (= 1 мс)
  Timer1.attachInterrupt(timerIsr);// запуск таймера


  // инициализация дисплея
  oled.init();  
  // очистить дисплей (или буфер)
  oled.clear();
  // заголовок и не меняющиеся надписи
  oled.home();// курсор в 0,0
  //oled.setScale(3);
  oled.print("ПЛАВИЛЬНЯ");
  oled.setCursorXY(0, 10); // курсор в (пиксель X, пиксель Y)
  oled.print("Тек/Уст ТЕМПЕРАТУРА");  
  
  oled.setCursorXY(0, 40); 
  oled.print("Скор. мотора");
  // обновить дисплей    
  oled.update();  


  // инициализация шагового двигателя
  stepper.enable();
  stepper.setMaxSpeed(100);     // скорость движения к цели
  stepper.setAcceleration(200); // ускорение
  stepper.reverse(true);
  

  //Serial.begin(9600);
}




// Таймер вывода инфы на дисплей
static uint32_t dspTmr;
void loop() {


  // Serial.print(enteredTemp - TEMP_VARIATE);
  // Serial.print("   ");
  // Serial.println(sensorTemp);

  // устанавливаем текущую скорость двигателя
  stepper.setSpeed(enteredMotorSpeed);  

  // Проверка датчика обрыва
  if(digitalRead(ROLLOUT_PIN)){
    // обрыв
    digitalWrite(HOT_PIN, 0);// выкл
    // выкл двигло
    stepper.disable();
  }else{
    // включаем двигатель
    stepper.enable();
    
    // поддержание температуры
    if(sensorTemp < enteredTemp - TEMP_VARIATE){
      digitalWrite(HOT_PIN, 1); // включить
    }
    if(sensorTemp > enteredTemp + TEMP_VARIATE){
      digitalWrite(HOT_PIN, 0); // выкл
    }
  }
  

  // раз в 30 милисекунд обновляем дисплей (вторая проверка для переполнения таймера)
  if(millis() - dspTmr > 300 || millis() < dspTmr){

    // Опрос датчика температуры
    sensorTemp = Thermistor(analogRead(A0));


    // -- вывод данных на дисплей --
    // температура
    oled.setCursorXY(0, 24); 
    oled.print(sensorTemp);   
    oled.print('/');  
    if(settingMode == 1){// Если включен режим настройки
      oled.invertText(true);
      oled.print(enteredTemp); 
      oled.invertText(false);
    } else
      oled.print(enteredTemp); 
    oled.print("       "); // Затирание символов в конце строки
    // Скорость
    oled.setCursorXY(0, 48);  
    if(settingMode == 2){// Если включен режим настройки
      oled.invertText(true);
      oled.print(enteredMotorSpeed*SPEED_COEF); 
      oled.invertText(false);
    } else
      oled.print(enteredMotorSpeed * SPEED_COEF);  
    oled.print("ММ/МИН(типа)    "); // Затирание символов в конце строки   
    oled.update();
  }


}









