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
// принятые данные
boolean doneParsing, startParsing;
int dataX, dataY;


void setup() 
{


  // инициализация bluetooth
  BTserial.begin(9600);


  // инициализация моторов
  motors_init();

  //motorL_drive(true, 200);
  //delay(300);
  //motorL_drive(true, 0);
  //delay(300);


  //motorR_drive(true, 200);
  //delay(300);
  //motorR_drive(true, 0);
  //delay(300);

  
  // motorL_drive(true, 200);// вперед
  // motorR_drive(true, 200);
  // delay(500);
  // motorL_drive(false, 200);// назад
  // motorR_drive(false, 200);
  // delay(500);

  // // поворот платформы обоими моторами
  // motors_rotate(false, 200);// влево
  // delay(500);
  // motors_rotate(true, 200);// вправо
  // delay(500);
  // motors_stop();

  
  Serial.begin(9600);
  pinMode(13, 1);
} 
 
void loop() 
{
  parsing();         // функция парсинга
  if (doneParsing) { // если получены данные
    doneParsing = false;

    int joystickX = map((dataX), -JOY_MAX, JOY_MAX, -MOTOR_MAX / 2, MOTOR_MAX / 2); // сигнал по Х
    int joystickY = map((dataY), -JOY_MAX, JOY_MAX, -MOTOR_MAX, MOTOR_MAX);         // сигнал по Y

    int dutyR = joystickY + joystickX; // считаем сигнал для правого мотора
    int dutyL = joystickY - joystickX; // считаем сигнал для левого мотора


    if(dutyR >= 0){
      motorR_drive(true, (uint8_t) dutyR);
    } else {
      motorR_drive(false, (uint8_t) (-dutyR));
    }

    if(dutyL >= 0){
      motorL_drive(true, (uint8_t) dutyL);
    } else {
      motorL_drive(false, (uint8_t) (-dutyL));
    }
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


/*
  Bluetooth шлёт пакет вида $valueX valueY;
  моя функция parsing() разбивает этот пакет в переменные
  Парсинг полностью прозрачный, не блокирующий и с защитой от битого пакета,
  так как присутствует строгий синтаксис посылки. Без хешсуммы конечно, но и так норм
*/
String string_convert;
void parsing() {
  if (BTserial.available() > 0) {       // если в буфере есть данные
    char incomingChar = BTserial.read();// читаем из буфера


    Serial.println(incomingChar);
    if(incomingChar == 'e'){
       digitalWrite(13, 1);
    }  
    if(incomingChar == 'd'){
       digitalWrite(13, 0);
    }  


    // if (startParsing) {                 // начать принятие пакетов
    //   if (incomingChar == ' ') {        // принят пакет dataX
    //     dataX = string_convert.toInt(); // ковертируем принятый пакет в переменную
    //     string_convert = "";            // очищаем переменную пакета
    //   }
    //   else if (incomingChar == ';') {   // принят пакет dataY
    //     dataY = string_convert.toInt(); // ковертируем принятый пакет в переменную
    //     string_convert = "";            // очищаем переменную пакета
    //     startParsing = false;           // закончить принятие пакетов
    //     doneParsing = true;             // парсинг окончен, можно переходить к движению
    //   } else {
    //     string_convert += incomingChar; // записываем  принятые данные в переменную
    //   }
    // }

    // if (incomingChar == '$') {          // начало парсинга
    //   startParsing = true;              // начать принятие пакетов
    // }
  }
}
















