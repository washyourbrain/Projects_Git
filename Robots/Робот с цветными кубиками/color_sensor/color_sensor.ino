// Пины подключения
#define PIN_S0 8
#define PIN_S1 9
#define PIN_S2 10
#define PIN_S4 11
#define PIN_Out 12

int red;
int green;
int blue;



void setup() {
  // сконфигурировать пины
  pinMode(PIN_S0, 1);
  pinMode(PIN_S1, 1);
  pinMode(PIN_S2, 1);
  pinMode(PIN_S4, 1);
  pinMode(PIN_Out, 0);

  // масштабирование 20%
  digitalWrite(PIN_S0,1);
  digitalWrite(PIN_S1,0);

  Serial.begin(9600);
}
void loop() {
  // установить R фильтр
  digitalWrite(PIN_S2,0);
  digitalWrite(PIN_S4,0);
  // Получение частоты на выходе
  red = pulseIn(PIN_Out, 0);
  // вывод в последовательный порт
  /*
  Serial.print("R=");
  Serial.print(red);
  Serial.print("=");
  Serial.print(30 < red && red < 80);
  */
  delay(80);

  
  // установить G фильтр
  digitalWrite(PIN_S2,1);
  digitalWrite(PIN_S4,1);
  // Получение частоты на выходе
  green = pulseIn(PIN_Out, 0);
  // вывод в последовательный порт
  /*
  Serial.print(" G=");
  Serial.print(green);
  Serial.print("=");
  Serial.print(40 < green && green < 70);
  */
  delay(80);

  
  // установить B фильтр
  digitalWrite(PIN_S2,0);
  digitalWrite(PIN_S4,1);
  // Получение частоты на выходе
  blue = pulseIn(PIN_Out, 0);
  // вывод в последовательный порт
  /*
  Serial.print(" B=");
  Serial.print(blue);
  Serial.print("=");
  Serial.print(45 < blue && blue < 100);
  */
  delay(80);

  //Serial.print(" ");

  getColor(red, green, blue);

  
  
}






int getColor(int red, int green, int blue){
  

  boolean isRed = 30 < red && red < 80;
  boolean isGreen = 40 < green && green < 70;
  boolean isBlue = 45 < blue && blue < 100;

  
  if(isRed && isGreen && !isBlue){//Желтый
    Serial.println("Yellow");
  }else if(!isRed && !isGreen && isBlue){// Синий
    Serial.println("Blue");
  }else if(isRed && !isGreen && !isBlue){// Красный
    Serial.println("Red");
  }else if(isGreen && !isBlue && !isRed){//Зелёный
    Serial.println("Green");
  }else{
    Serial.println("Empty");
  }
}