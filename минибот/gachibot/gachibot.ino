 
void setup() {
  // инициализация моторов
  motors_init();

  //motor_1(true, 200);
  //delay(300);
  //motor_1(true, 0);
  //delay(300);


  //motor_2(true, 200);
  //delay(300);
  //motor_2(true, 0);
  //delay(300);

  // поворот платформы обоими моторами
  motor_rotate(false, 50);
  delay(1000);
  motor_rotate(true, 0);
} 
 
void loop() {

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

// поворот платформы обоими моторами
void motor_rotate(bool isRight,  uint8_t speed)
{// true - вправо(по часовой), false - влево(против часовой), speed число в диапазоне от 0 до 255
  //motor_1(!isLeft, speed);
  //motor_2(isLeft, speed);
  speed = (speed < DEAD_ZONE)?(0):(speed); 
  digitalWrite(4, isRight); 
  digitalWrite(7, isRight); 
  analogWrite(5, speed); 
  analogWrite(6, speed); 
}

