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
// принятые данные
//boolean doneParsing, startParsing;


void setup() {

  // инициализация bluetooth
  BTserial.begin(9600);

  // инициализация моторов
  motors_init();


  Serial.begin(9600);
  pinMode(13, 1);
}



int dataX, dataY;

void loop() {
  parsing();  // функция парсинга




  // if (doneParsing) {  // если получены данные
  //   doneParsing = false;

  //   int joystickX = map((dataX), -JOY_MAX, JOY_MAX, -MOTOR_MAX / 2, MOTOR_MAX / 2);  // сигнал по Х (-128 <> 127)
  //   int joystickY = map((dataY), -JOY_MAX, JOY_MAX, -MOTOR_MAX, MOTOR_MAX);          // сигнал по Y (-256 <> 255)

  //   int dutyR = joystickY + joystickX;  // считаем сигнал для правого мотора
  //   int dutyL = joystickY - joystickX;  // считаем сигнал для левого мотора


  //   if (dutyR >= 0) {
  //     motorR_drive(true, (uint8_t)dutyR);
  //   } else {
  //     motorR_drive(false, (uint8_t)(-dutyR));
  //   }

  //   if (dutyL >= 0) {
  //     motorL_drive(true, (uint8_t)dutyL);
  //   } else {
  //     motorL_drive(false, (uint8_t)(-dutyL));
  //   }
  // }
}



bool isReadNumber = false;
uint8_t tempNumber = 0;

int lmot;
int rmot;

void parsing() {
  if (BTserial.available() > 0) {         // если в буфере есть данные
    char incomingChar = BTserial.read();  // читаем из буфера

    Serial.println(incomingChar);



    if (isReadNumber && (incomingChar) <= '9' && (incomingChar) >= '0') {  // '0' == 48
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
        default:
          break;
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
