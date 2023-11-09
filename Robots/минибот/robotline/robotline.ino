

// === Константы =========================================================
#define LEFT_SCANNER_IN 10
#define RIGHT_SCANNER_IN 11

#define BUTTON 8
#define LEFT_SCANNER_ANALOG_IN A0
#define RIGHT_SCANNER_ANALOG_IN A1

// === Динамичесике переменные =========================================================
int whiteValueL;
int blackValueL;
int middleValueL;

int whiteValueR;
int blackValueR;
int middleValueR;



void setup() {
  // инициализация моторов
  motors_init();

  pinMode(LEFT_SCANNER_IN, INPUT);
  pinMode(RIGHT_SCANNER_IN, INPUT);

  pinMode(LEFT_SCANNER_ANALOG_IN, INPUT);
  pinMode(RIGHT_SCANNER_ANALOG_IN, INPUT);

  pinMode(BUTTON, INPUT_PULLUP);




// калибровка

  Serial.begin(9600);

  // пока кнопка не нажата
  while(digitalRead(BUTTON));
  delay(10);
  // пока кнопка не отпущена
  while(!digitalRead(BUTTON));
  delay(10);

  // считываем значение белого цвета
  Serial.print(analogRead(LEFT_SCANNER_ANALOG_IN));
  whiteValueL = analogRead(LEFT_SCANNER_ANALOG_IN);
  whiteValueR = analogRead(RIGHT_SCANNER_ANALOG_IN);


  // пока кнопка не нажата
  while(digitalRead(BUTTON));
  delay(10);
  // пока кнопка не отпущена
  while(!digitalRead(BUTTON));
  delay(10);

  // считываем значение белого цвета
  blackValueL = analogRead(LEFT_SCANNER_ANALOG_IN);
  blackValueR = analogRead(RIGHT_SCANNER_ANALOG_IN);

  Serial.print("whiteValueL");
  Serial.print(whiteValueL);
  Serial.print("  blackValueL");
  Serial.println(blackValueL);

  Serial.print("whiteValueR");
  Serial.print(whiteValueR);
  Serial.print("  blackValueR");
  Serial.println(blackValueR);

  middleValueL = (blackValueL - whiteValueL) / 2 + whiteValueL;
  middleValueR = (blackValueR - whiteValueR) / 2 + whiteValueR;


} 

int temp;
 
void loop() {


  
  bool dLeft = analogRead(LEFT_SCANNER_ANALOG_IN) > middleValueL;
  bool dRight = analogRead(RIGHT_SCANNER_ANALOG_IN) > middleValueR;


  // Serial.print(analogRead(LEFT_SCANNER_ANALOG_IN));
  // Serial.print("  ");
  // Serial.print(middleValueL);
  // Serial.print("  ");
  // Serial.println(dLeft);

  // delay(100);

  //  Serial.print(dLeft);
  //  Serial.print(" ");
  //  Serial.println(dRight);

  if(dLeft && dRight){
    motor_2(false, 0);
    motor_1(false, 0);
  } else if(dLeft){
    motor_2(false, 50);
    motor_1(true, 50);
  } else if(dRight){
    motor_2(true, 50);
    motor_1(false, 50);
    
  } else {
    motor_2(false, 50);
    motor_1(false, 50);
  }

}


// ------------------------------------------------------------------------
// ------------------------ Вспомогательные методы ------------------------
// ------------------------------------------------------------------------

// мертвая зона чтобы не спалить движки
#define DEAD_ZONE 50

void motors_init()
{// инициализация моторов для setup, ОБЯЗАТЕЛЬНА
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

// пуск мотора 1 со скоростю и направлением
void motor_1(bool isForward, uint8_t speed)
{// true - вперед, false - назад, speed число в диапазоне от 0 до 255 (0 - стоп)
  digitalWrite(4, !isForward);
  analogWrite(5, (speed < DEAD_ZONE)?(0):(speed)); 
}

// пуск мотора 2 со скоростю и направлением
void motor_2(bool isForward, uint8_t speed)
{// true - вперед, false - назад, speed число в диапазоне от 0 до 255 (0 - стоп)
  digitalWrite(7, isForward); 
  analogWrite(6, (speed < DEAD_ZONE)?(0):(speed)); 
}

