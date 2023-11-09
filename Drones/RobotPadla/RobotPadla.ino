// выходы на управление моторами
#define MOTORL_1 5 
#define MOTORL_2 3

#define MOTORR_1 9
#define MOTORR_2 6


/*============= НАСТРОЙКИ =============*/
#define MOTOR_MAX 255                 // максимальный сигнал на мотор (max 255)
#define JOY_MAX 40                    // рабочий ход джойстика (из приложения)
// мертвая зона чтобы не спалить движки
#define DEAD_ZONE 50



// Текущая скорость
uint8_t speed = 200;

unsigned long lastTimerCheck;
int mode = 0;

void setup() 
{
  // инициализация моторов
  motors_init();

  
  pinMode(13, 1);

  // перемычка остановки работы
  pinMode(12, 0);
  digitalWrite(12, 1);

  lastTimerCheck = millis();

} 



 
void loop() 
{

  // перемычка остановки работы
  if(digitalRead(12) == 0){

    // работа

    unsigned long currentTime = millis();
    if(currentTime - lastTimerCheck >= 2000){
      lastTimerCheck = currentTime;
      mode++;
      if(mode > 3) mode = 0;
      
    

    switch(mode){
      case 0:// Лево-Вперёд

        digitalWrite(MOTORL_1, 1);
        digitalWrite(MOTORL_2, 1);
        digitalWrite(MOTORR_1, 1);
        digitalWrite(MOTORR_2, 1);
        
        digitalWrite(13, 1);

        //motorL_drive(true, 0);
        //motorR_drive(true, speed);
        break;
      case 1:// Вперёд
      
        digitalWrite(MOTORL_1, 0);
        digitalWrite(MOTORL_2, 1);
        digitalWrite(MOTORR_1, 0);
        digitalWrite(MOTORR_2, 1);

        
        digitalWrite(13, 0);
        //motorL_drive(true, speed);
        //motorR_drive(true, speed);
        break;
      case 2:// Право-Вперёд
        digitalWrite(MOTORL_1, 1);
        digitalWrite(MOTORL_2, 0);
        digitalWrite(MOTORR_1, 1);
        digitalWrite(MOTORR_2, 0);

        
        digitalWrite(13, 1);
        //motorL_drive(true, speed);
        //motorR_drive(true, 0);
        break;
      case 3:// Право-Назад
        digitalWrite(MOTORL_1, 1);
        digitalWrite(MOTORL_2, 1);
        digitalWrite(MOTORR_1, 1);
        digitalWrite(MOTORR_2, 1);

        
        digitalWrite(13, 0);
        //motorL_drive(true, 0);
        //motorR_drive(false, speed);
        break;
      //case 4:// Назад
        //motorL_drive(false, speed);
        //motorR_drive(false, speed);
        break;
      //case 5:// Лево-Назад
        //motorL_drive(false, speed);
        //motorR_drive(false, 0);
        break;
    }
    }

  }else{
    // отключение двигателей 
    motors_stop();
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
  digitalWrite(MOTORL_1, 1);
  digitalWrite(MOTORL_2, 1);
  digitalWrite(MOTORR_1, 1);
  digitalWrite(MOTORR_2, 1);
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
void motors_stop()
{// true - вправо(по часовой), false - влево(против часовой), speed число в диапазоне от 0 до 255
  digitalWrite(MOTORL_1, 1);
  digitalWrite(MOTORL_2, 1);
  digitalWrite(MOTORR_1, 1);
  digitalWrite(MOTORR_2, 1); 
}














