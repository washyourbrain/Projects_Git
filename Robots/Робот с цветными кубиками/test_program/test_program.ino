#define SPEED_LEFT      5
#define SPEED_RIGHT     6 
#define DIR_LEFT        4
#define DIR_RIGHT       7

// определение цвета
#define B_EMPTY -1
#define B_RED 0
#define B_BLUE 1
#define B_YELLOW 2


//пины датчика цвета
#define PIN_S0 8
#define PIN_S1 9
#define PIN_S2 10
#define PIN_S4 11
#define PIN_Out 12

void setup() {
  pinMode(A0, INPUT); //левый
  pinMode(A1, INPUT); //правый
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  

  Serial.begin(9600);
}

void loop() {
  Serial.print("left:"); Serial.print(analogRead(A0));
  Serial.print("right:"); Serial.println(analogRead(A1));
  delay(80);

  if (analogRead(A0) < 26 && analogRead(A1) < 26) { //езда прямо
    go(30, 0, 0);
    delay(10);
  }

  
  if (analogRead(A0) >= 26 && analogRead(A1) >= 26) { //остановка на линии, ожидание действия
    delay(450);
    go(0, 0, 0);
    delay(1500);
    tank180(1);
    delay(1500);
  }

  if (analogRead(A0) >= 26 && analogRead(A1) < 26) { //поворот налево
    goWheel(35, 15, 0, 0);
    delay(10);
  }

  if (analogRead(A1) >= 26 && analogRead(A1) < 26) { //поворот направо
    goWheel(15, 35, 0, 0);
    delay(10);
  }
}




void go(int speed, bool reverseLeft, bool reverseRight)
{
  // Для регулировки скорости speed может принимать значения от 0 до 255,
  // чем болше, тем быстрее. 
  analogWrite(SPEED_LEFT, speed);
  analogWrite(SPEED_RIGHT, speed);
  digitalWrite(DIR_LEFT, !reverseLeft ? LOW : HIGH); 
  digitalWrite(DIR_RIGHT, reverseRight ? LOW : HIGH); 
}


void goWheel(int speedLeft, int speedRight, bool reverseLeft, bool reverseRight)
{
  analogWrite(SPEED_LEFT, speedLeft);
  analogWrite(SPEED_RIGHT, speedRight);
  digitalWrite(DIR_LEFT, !reverseLeft ? LOW : HIGH); 
  digitalWrite(DIR_RIGHT, reverseRight ? LOW : HIGH); 
}




void tank180(bool side) //side - сторона, HIGH - лево, LOW - право
{
  analogWrite(SPEED_LEFT, 40);
  analogWrite(SPEED_RIGHT, 40);
  digitalWrite(DIR_LEFT, side ? LOW : HIGH); 
  digitalWrite(DIR_RIGHT, side ? LOW : HIGH); 
  delay(620);
  analogWrite(SPEED_LEFT, 0);
  analogWrite(SPEED_RIGHT, 0);
}