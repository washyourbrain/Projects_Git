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

---- hotend
 44
 45
 46
 
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

// ---------------- подключение портов ---------------------

// датчик обрыва
#define ROLLOUT_PIN 9

// энкодер
#define CLK 22 // желт
#define DT 24  // оранж
#define ENC_SW 26  // красн

// шаговые двигатели
#define STP_PIN_EN 8 // 8 пин шилда
// x stepper
#define STP1_PIN_ST 2  // (2 пин шилда)
#define STP1_PIN_DIR 5 // (5 пин шилда)

// транзисторы нагрева
#define HOT_PIN_0 44
#define HOT_PIN_1 45
#define HOT_PIN_2 46

// датчики нагрева
#define THERMISTOR_PIN A8





// ---------------- Основные параметры плавильни ---------------- 

// Настроенная температура  от TEMP_MIN, до TEMP_MAX  
#define TEMP_MIN 20
#define TEMP_MAX 500
static uint16_t enteredTemp = 150;

// диапазон допустимых колебаний температуры при плавке
// например 4 - это +-4 градуса
#define TEMP_VARIATE 2

// Температура с датчика
float sensorTemp = 0;

// Текущая скорость мотора
#define SPEED_COEF 1 // коэффициэнт пресчета (столько мм в минуту дает однаединица скорости)
#define SPEED_MIN 400
#define SPEED_MAX 2000
int enteredMotorSpeed = 998;

// Режим настройки 0 - ничего, 1 - настройка температуры
static uint8_t settingMode = 0;


// ---------------- Подключенный обвес ---------------- 

// -- Подключение энкодера --
Encoder enc1(CLK, DT, ENC_SW);
// -- Подключение шаговика --
GStepper2<STEPPER2WIRE> stepper(2048, STP1_PIN_ST, STP1_PIN_DIR, STP_PIN_EN);// step, dir, en
// -- Подключение дисплея --
GyverOLED<SSH1106_128x64> oled;



// -- Термистор --
#define THERMISTOR_B 4514 // B-коэффициент 3950
#define THERMISTOR_SERIAL_R 4840 // сопротивление последовательного резистора, 102 кОм    5K1
#define THERMISTOR_R 120400 // номинальное сопротивления термистора, 100 кОм
#define THERMISTOR_NOMINAL_T 21 // номинальная температура (при которой TR = 100 кОм) 25


float resistanseCurrent;
float steinhart;
float getTemp() {
    //int t = analogRead( THERMISTOR_PIN );   expRunningAverage
    resistanseCurrent = 1023.0 / (analogRead( THERMISTOR_PIN )) - 1;
    resistanseCurrent = THERMISTOR_SERIAL_R / resistanseCurrent;

    steinhart = log(resistanseCurrent / THERMISTOR_R) / THERMISTOR_B; // 1/B * log(R/Ro)
    steinhart += 1.0 / (THERMISTOR_NOMINAL_T + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart; // Invert
    steinhart -= 273.15; 

    // Serial.print(analogRead( THERMISTOR_PIN ));
    // Serial.print(" ");
    // Serial.print( steinhart );
    // Serial.print(" ");

    // Serial.print("50 ");
    // Serial.print(analogRead(THERMISTOR_PIN));
    // Serial.print(" ");
    // Serial.println(steinhart);

    return steinhart;
}










// ---------------- Прерывания ---------------- 

// опрос энкодера и кнопок
bool lastButtonstate = false;
void timerIsr() {  // прерывание таймера

  // обработка энкодера
  enc1.tick();
  // изменяем значения при повороте энкодера
  // Настройка температуры
  if(settingMode == 1){
    if (enc1.isRight()) { enteredTemp = (enteredTemp < TEMP_MAX)?(enteredTemp + 1):(TEMP_MAX); }      
    if (enc1.isLeft()) { enteredTemp = (enteredTemp > TEMP_MIN)?(enteredTemp - 1):(TEMP_MIN); }
  }else 
  // Настройка скорости
  if(settingMode == 2){
    if (enc1.isRight()) { enteredMotorSpeed = (enteredMotorSpeed < SPEED_MAX)?(enteredMotorSpeed + 10):(SPEED_MAX); }      
    if (enc1.isLeft()) { enteredMotorSpeed = (enteredMotorSpeed > SPEED_MIN)?(enteredMotorSpeed - 10):(SPEED_MIN); }
  }
  // если было удержание + поворот
  //if (enc1.isRightH()) Serial.println("Right holded"); 
  //if (enc1.isLeftH()) Serial.println("Left holded");


  // Нажатие на кнопку
  // Если кнопка была отпущена и сейчас нажата 
  if(!lastButtonstate && !digitalRead(ENC_SW)){
    lastButtonstate = true;
  } else
  // Если кнопка была нажата и сейчас отпущена 
  if(lastButtonstate && digitalRead(ENC_SW)){
    lastButtonstate = false;  
    settingMode = (settingMode >= 2)?(0):(settingMode + 1);
  }


  // обработка шагового двигателя 
  // здесь происходит движение моторов, вызывать как можно чаще
  stepper.tick();


  // Обработка датчика обрыва
}



// ---------------- Дополнительные функции -----------------

// пид регулятор
float integral = 0, prevErr = 0;
#define PID_P 6.5 // P-коэффициент 65
#define PID_I 1.0 // I-коэффициент 1
#define PID_D 3.1 // D-коэффициент
#define PID_DT_DELAY 100 // Время в милисекундах
#define PID_DT 0.1f // Время в секундах

// (вход, установка, п, и, д, период в секундах, мин.выход, макс. выход)
int computePID(float input, float setpoint, float kp, float ki, float kd, float dt, int minOut, int maxOut) {
  float err = setpoint - input;
  integral = constrain(integral + (float) err * dt * ki, minOut, maxOut);
  float D = (err - prevErr) / dt;
  prevErr = err;
  return constrain(err * kp + integral + D * kd, minOut, maxOut);
}


// фильтр значений датчика температуры
float k = 0.3;  // коэффициент фильтрации, 0.0-1.0
float filVal = 0;
// бегущее среднее
float expRunningAverage(float newVal) {
  filVal += (newVal - filVal) * k;
  return filVal;
}

// ---------------- Основная программа ---------------- 


void setup() {
  // инициализация транзистора нагрева
  pinMode(HOT_PIN_0, OUTPUT);
  pinMode(HOT_PIN_1, OUTPUT);
  pinMode(HOT_PIN_2, OUTPUT);
  digitalWrite(HOT_PIN_0, 0);
  digitalWrite(HOT_PIN_1, 0);
  digitalWrite(HOT_PIN_2, 0);

  // инициализация датчика обрыва
  pinMode(ROLLOUT_PIN, INPUT);
  digitalWrite(ROLLOUT_PIN, 1);

  
  // инициализация термистора
  pinMode( THERMISTOR_PIN, INPUT );

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
  
  // инициализация фильтра температуры
  filVal = analogRead( THERMISTOR_PIN );
  sensorTemp = getTemp();

  Serial.begin(9600);
}




// Таймер вывода инфы на дисплей
static uint32_t dspTmr;
// Таймер вызова PID функции
static uint32_t pidTmr;
void loop() {


  // Serial.print(enteredTemp - TEMP_VARIATE);
  // Serial.print("   ");
  // Serial.println(sensorTemp);

  // устанавливаем текущую скорость двигателя
  stepper.setSpeed(enteredMotorSpeed);  

  // Проверка датчика обрыва
  if(digitalRead(ROLLOUT_PIN)){// обрыв
    // выкл двигло
    stepper.disable();
  }else{
    // включаем двигатель
    stepper.enable();
    
  }

  // раз в установленый период DT просчитываем пид регулятор (вторая проверка для переполнения таймера)
  if(millis() - pidTmr > PID_DT_DELAY || millis() < pidTmr){
    pidTmr = millis();

    
    // Опрос датчика температуры
    sensorTemp = getTemp();//expRunningAverage(getTemp());
    
    // (вход, установка, п, и, д, период в секундах, мин.выход, макс. выход)
    int output = computePID(sensorTemp, enteredTemp, PID_P, PID_I, PID_D, PID_DT, 0, 255);
    analogWrite(HOT_PIN_0, output);

    // для настройки PID
    //Serial.print("in=");
    Serial.print(enteredTemp);
    //Serial.print("out=");
    Serial.print(" ");
    Serial.print(sensorTemp);
    Serial.print(" ");
    Serial.println(output);

  }


  // Serial.print("50 ");
  // Serial.println(expRunningAverage(getTemp()));

  
  // // поддержание температуры
  // if(sensorTemp < enteredTemp - TEMP_VARIATE){
  //   digitalWrite(HOT_PIN_0, 1); // включить
  // }
  // if(sensorTemp > enteredTemp + TEMP_VARIATE){
  //   digitalWrite(HOT_PIN_0, 0); // выкл
  // }
  

  // раз в 30 милисекунд обновляем дисплей (вторая проверка для переполнения таймера)
  if(millis() - dspTmr > 300 || millis() < dspTmr){
    dspTmr = millis();


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
      oled.print(enteredMotorSpeed * SPEED_COEF); 
      oled.invertText(false);
    } else
      oled.print(enteredMotorSpeed * SPEED_COEF);  
    oled.print(" попугаев        "); // Затирание символов в конце строки   
    oled.update();
  }


}









