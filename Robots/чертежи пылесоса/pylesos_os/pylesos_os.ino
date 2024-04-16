// выходы на управление моторами
#define MOTORL_1 3
#define MOTORL_2 5

#define MOTORR_1 6
#define MOTORR_2 9


/*============= НАСТРОЙКИ =============*/
//#define MOTOR_MAX 255  // максимальный сигнал на мотор (max 255)
//#define JOY_MAX 40     // рабочий ход джойстика (из приложения)
// мертвая зона чтобы не спалить движки
#define DEAD_ZONE 50

// подключение модуля bluetooth
#include <SoftwareSerial.h>
#define BT_TX 8
#define BT_RX 7
SoftwareSerial BTserial(BT_TX, BT_RX);  // TX, RX 

// установленный режим автономии 
//  0 - ручной
//  1 - печной, всмысле квадрат
//  2 - круг 
int rideMode = 0;
// текущая задержка для таймера автономии
unsigned long  currentAutoDelay = 0;
// время начала работы автономии для таймера автономии
unsigned long  currentAutoTime = 0;
// текущая команда для автономии
int currentAutoComandPos = 0;
// максимальное количество автономных команд
#define MAX_COMANDS_COUNT 10
// скорость моторов в автономе
#define AUTONOME_MOTOR_SPEED 255


void setup() {

  // инициализация bluetooth
  BTserial.begin(9600);

  // инициализация моторов
  motors_init();

  Serial.begin(9600);
  pinMode(13, 1);

}



void loop() {


  // режим автономии 
  if(rideMode != 0){

    // пора тихоходке выполнить новую команду
    if (millis() - currentAutoTime >= currentAutoDelay) {

      // режим автономии 1
      if(rideMode == 1){


        // движения по квадрату
        switch(currentAutoComandPos){
          case 0:
            // настраиваем время выполнения команды
            currentAutoDelay = 2000;
            // включаем моторы
            motorR_drive(true, AUTONOME_MOTOR_SPEED);
            motorL_drive(true, AUTONOME_MOTOR_SPEED);
            break;

          case 1:
            // настраиваем время выполнения команды
            currentAutoDelay = 450;
            // включаем моторы
            motorR_drive(false, AUTONOME_MOTOR_SPEED);
            motorL_drive(true, AUTONOME_MOTOR_SPEED);
            break;

          case 2:
            // остановим моторы
            motors_stop();
            // задержка до следующего повтора 
            currentAutoDelay = 1000;
            break;
            
          case 3:
            // конец последовательности (эта строка должна быть в последнем кейсе)
            currentAutoDelay = 0;
        }



      }else if(rideMode == 2){// режим автономии 2



        // режим движения по кругу
        switch(currentAutoComandPos){
          case 0:
            // настраиваем время выполнения команды
            currentAutoDelay = 450;
            // включаем моторы
            motorR_drive(true, AUTONOME_MOTOR_SPEED-30);
            motorL_drive(true, AUTONOME_MOTOR_SPEED);
            break;

          case 1:
            // конец последовательности (эта строка должна быть в последнем кейсе)
            currentAutoDelay = 0;
        }



      }


      // Если последовательность закончилась
      if(currentAutoComandPos >= MAX_COMANDS_COUNT){
          // к первой команде
          currentAutoComandPos = 0;

      }else{
        // следующая команда
        currentAutoComandPos++;
      }
      
      // стартуем таймер ожидания
      currentAutoTime = millis();
    }
  }

  // функция парсинга команд с bluetooth
  parsing();  

}



bool isReadNumber = false;
uint8_t tempNumber = 0;

// текущая скорость моторов настроенная с помощью ползунка
int lmot = 255;
int rmot = 255;


void parsing() {
  if (BTserial.available() > 0) {         // если в буфере есть данные
    char incomingChar = BTserial.read();  // читаем из буфера

    // отладочный вывод
    Serial.println(incomingChar);

    // ============== реагирование на перемещение ползунка ==============

    // если сейчас читаем число и на входе цифра (чтение ползунка)
    if (isReadNumber && (incomingChar) <= '9' && (incomingChar) >= '0') {  // '0' == 48
      // прибавляем ее к уже считанному числу
      tempNumber = tempNumber * 10 + (incomingChar - '0');
    } else {

      // если читалось число заканчиваем его чтение
      if (isReadNumber) {

        // рулим
        if (tempNumber <= 90) {  // влево
          lmot = map(tempNumber, 0, 90, -255, 255);
          rmot = 255;
        } else {  // вправо
          lmot = 255;
          rmot = map(tempNumber, 91, 180, 255, -255);
        }
        // вот и порулили 

        Serial.println(tempNumber);
        isReadNumber = false;
        tempNumber = 0;
      }



      // ============== реагирование на нажатие кнопок ==============

      // если режим автономии выключен
      if(rideMode == 0){

        switch (incomingChar) {
          case 'S':  // стоп
            motors_stop();

            break;
          case 'F':  // вперед

            if (lmot < 0) {
              motorL_drive(false, (uint8_t)(-lmot));
            } else {
              motorL_drive(true, (uint8_t)(lmot));
            }
            if (rmot < 0) {
              motorR_drive(false, (uint8_t)(-rmot));
            } else {
              motorR_drive(true, (uint8_t)(rmot));
            }
            break;
          case 'G':  // назад
            if (lmot < 0) {
              motorL_drive(true, (uint8_t)(-lmot));
            } else {
              motorL_drive(false, (uint8_t)(lmot));
            }
            if (rmot < 0) {
              motorR_drive(true, (uint8_t)(-rmot));
            } else {
              motorR_drive(false, (uint8_t)(rmot));
            }

            break;
          case 'L':  // лево 100
            motorR_drive(true, 255);
            motorL_drive(false, 255);

            break;
          case 'R':  // право 100
            motorR_drive(false, 255);
            motorL_drive(true, 255);
            break;
          case 'K':  // читаем поворот
            isReadNumber = true;
            break;
          case 'X':  // включаем режим автономии 1
            rideMode = 1;
            motors_stop();
            currentAutoComandPos = 0;
            currentAutoDelay = 0;
            break;
          case 'Y':  // включаем режим автономии 2
            rideMode = 2;
            motors_stop();
            currentAutoComandPos = 0;
            currentAutoDelay = 0;
            break;
          default:
            break;
        }

      }else if(rideMode == 1){// если включен режим автономии 1
        // реагируем только на одну кнопку
        if(incomingChar == 'X'){
          // отключение режима автономии
          rideMode = 0;
          motors_stop();
        }
      }else if(rideMode == 2){// если включен режим автономии 2
        // реагируем только на одну кнопку
        if(incomingChar == 'Y'){
          // отключение режима автономии
          rideMode = 0;
            motors_stop();
        }
          
      }


    }
  }
}








// ------------------------------------------------------------------------
// ------------------------ Вспомогательные методы ------------------------
// ------------------------------------------------------------------------


void motors_init() {
  // инициализация моторов для setup, ОБЯЗАТЕЛЬНА
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
void motorL_drive(bool isForward, uint8_t speed) {
  // true - вперед, false - назад, speed число в диапазоне от 0 до 255 (0 - стоп)

  speed = (speed < DEAD_ZONE) ? (0) : (speed);

  analogWrite((isForward) ? (MOTORL_2) : (MOTORL_1), 0);
  analogWrite((isForward) ? (MOTORL_1) : (MOTORL_2), speed);
}

// пуск мотора 2 со скоростю и направлением
void motorR_drive(bool isForward, uint8_t speed) {
  // true - вперед, false - назад, speed число в диапазоне от 0 до 255 (0 - стоп)
  speed = (speed < DEAD_ZONE) ? (0) : (speed);

  analogWrite((isForward) ? (MOTORR_2) : (MOTORR_1), 0);
  analogWrite((isForward) ? (MOTORR_1) : (MOTORR_2), speed);
}

// поворот платформы обоими моторами
void motors_rotate(bool isRight, uint8_t speed) {
  // true - вправо(по часовой), false - влево(против часовой), speed число в диапазоне от 0 до 255
  motorL_drive(isRight, speed);
  motorR_drive(!isRight, speed);
}

// поворот платформы обоими моторами
void motors_stop() {
  // true - вправо(по часовой), false - влево(против часовой), speed число в диапазоне от 0 до 255
  digitalWrite(MOTORL_1, 0);
  digitalWrite(MOTORL_2, 0);
  digitalWrite(MOTORR_1, 0);
  digitalWrite(MOTORR_2, 0);
}
