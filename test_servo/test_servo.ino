#include <ServoSmooth.h>

ServoSmooth servo_rotate;
ServoSmooth servo_vertical;
ServoSmooth servo_horizontal;
ServoSmooth servo_zahv;

// таймеры
unsigned long sheduleTimer;
int sheduleState = 0;

void setup() {
  // put your setup code here, to run once:

  servo_rotate.attach(9);//0-180
  servo_rotate.setSpeed(50);   // ограничить скорость
  servo_rotate.setAccel(0.1);
  servo_rotate.setTargetDeg(0);
  servo_rotate.setAutoDetach(false);
  servo_rotate.smoothStart();

  // servo_vertical.attach(3);
  // servo_vertical.setSpeed(50);   // ограничить скорость
  // servo_vertical.setAutoDetach(false);

  //smoothStart();


  sheduleTimer = millis();

  Serial.begin(9600);
}

void loop() {
  servo_rotate.tick();   // здесь происходит движение серво по встроенному таймеру!
  delay(10);
  //servo_vertical.tick();   
  
  //  вызов событий по расписанию
  if(millis() - sheduleTimer >= 1000){
    sheduleTimer = millis();

    // Счетчик событий
    sheduleState = (sheduleState >= 1) ? (0):(sheduleState + 1);
    
    // собсна сами события
    servo_rotate.setTargetDeg(30 * sheduleState);//0-180

  }
  Serial.println(servo_rotate.getCurrentDeg());
}
