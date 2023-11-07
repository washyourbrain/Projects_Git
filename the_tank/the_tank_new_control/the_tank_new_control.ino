// выходы на управление моторами
#define MOTORL_1 10 
#define MOTORR_1 5

#define MOTORL_2 11
#define MOTORR_2 6


/*============= НАСТРОЙКИ =============*/
#define MOTOR_MAX 255                 // максимальный сигнал на мотор (max 255)
#define JOY_MAX 40                    // рабочий ход джойстика (из приложения)
// мертвая зона чтобы не спалить движки
#define DEAD_ZONE 50

// подключение модуля bluetooth
#include <SoftwareSerial.h>
#define BT_TX 8
#define BT_RX 7
SoftwareSerial BTserial(BT_TX, BT_RX); // TX, RX


// Текущая скорость
uint8_t speed = 200;


void setup() 
{
  // инициализация bluetooth
  BTserial.begin(9600);
  // инициализация моторов
  motors_init();

  // Serial.begin(9600);
  // pinMode(13, 1);
} 
 
void loop() 
{
  // получение данных bluetooth
  if (BTserial.available() > 0) {       // если в буфере есть данные
    
    char incomingChar = BTserial.read();// читаем из буфера
    switch(incomingChar){
      case 'Q':// Лево-Вперёд
        motorL_drive(true, 0);
        motorR_drive(true, speed);
        break;
      case 'F':// Вперёд
        motorL_drive(true, speed);
        motorR_drive(true, speed);
        break;
      case 'E':// Право-Вперёд
        motorL_drive(true, speed);
        motorR_drive(true, 0);
        break;
      case 'R':// Право
        motorL_drive(true, speed);
        motorR_drive(false, speed);
        break;
      case 'C':// Право-Назад
        motorL_drive(true, 0);
        motorR_drive(false, speed);
        break;
      case 'G':// Назад
        motorL_drive(false, speed);
        motorR_drive(false, speed);
        break;
      case 'Z':// Лево-Назад
        motorL_drive(false, speed);
        motorR_drive(false, 0);
        break;
      case 'L':// Лево
        motorL_drive(false, speed);
        motorR_drive(true, speed);
        break;
      case 'S':// Стоп
        motors_stop();
        break;
    }

    // Serial.println(incomingChar);
    // if(incomingChar == 'e'){
    //    digitalWrite(13, 1);
    // }  
    // if(incomingChar == 'd'){
    //    digitalWrite(13, 0);
    // } 

  }
}














// ------------------------------------------------------------------------
// ------------------------ Вспомогательные методы ------------------------
// ------------------------------------------------------------------------


void motors_init()
{// инициализация моторов для setup, ОБЯЗАТЕЛЬНА
  pinMode(MOTORL_1, OUTPUT);
  pinMode(MOTORL_2, OUTPUT);
  pinMode(MOTORR_1, OUTPUT);
  pinMode(MOTORR_2, OUTPUT);
  digitalWrite(MOTORL_1, 0);
  digitalWrite(MOTORL_2, 0);
  digitalWrite(MOTORR_1, 0);
  digitalWrite(MOTORR_2, 0);
}

// пуск мотора 1 со скоростю и направлением
void motorL_drive(bool isForward, uint8_t speed)
{// true - вперед, false - назад, speed число в диапазоне от 0 до 255 (0 - стоп)
  
  speed = (speed < DEAD_ZONE)?(0):(speed); 

  analogWrite((isForward)?(MOTORL_2):(MOTORL_1), 0);
  analogWrite((isForward)?(MOTORL_1):(MOTORL_2), speed);    

}

// пуск мотора 2 со скоростю и направлением
void motorR_drive(bool isForward, uint8_t speed)
{// true - вперед, false - назад, speed число в диапазоне от 0 до 255 (0 - стоп)
  speed = (speed < DEAD_ZONE)?(0):(speed); 
  
  analogWrite((isForward)?(MOTORR_2):(MOTORR_1), 0);
  analogWrite((isForward)?(MOTORR_1):(MOTORR_2), speed);
}

// поворот платформы обоими моторами
void motors_rotate(bool isRight,  uint8_t speed)
{// true - вправо(по часовой), false - влево(против часовой), speed число в диапазоне от 0 до 255
  motorL_drive(isRight, speed);
  motorR_drive(!isRight, speed);
}

// поворот платформы обоими моторами
void motors_stop()
{// true - вправо(по часовой), false - влево(против часовой), speed число в диапазоне от 0 до 255
  digitalWrite(MOTORL_1, 0);
  digitalWrite(MOTORL_2, 0);
  digitalWrite(MOTORR_1, 0);
  digitalWrite(MOTORR_2, 0); 
}














