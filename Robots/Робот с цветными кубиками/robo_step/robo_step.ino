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


// #define DEBUG


// считанные цвета
int colors[10];

// текущая команда
int currentComand = 0;



//диаметр колеса 4.1 см
//12.87 см длина окружности

//скорость - 50, delay - 1000
//37,5 см

//2000 delay - 75 см
//267 delay - 10 см
//134 delay - 5 см
//27 delay - 1 см

//1 оборот - 343 delay 

//танковый 180 - 40 скорость, 615 delay (каждый)
//танковый 90 - 40 скорость, 350 delay (каждый)

//чёрный ~33
//белый ~25
//среднее 29



void setup() 
{
  // Настраивает выводы платы 4,5,6,7 на вывод сигналов 
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A0, INPUT); //левый
  pinMode(A1, INPUT); //правый





  // сконфигурировать пины
  pinMode(PIN_S0, 1);
  pinMode(PIN_S1, 1);
  pinMode(PIN_S2, 1);
  pinMode(PIN_S4, 1);
  pinMode(PIN_Out, 0);

  // масштабирование 20%
  digitalWrite(PIN_S0,1);
  digitalWrite(PIN_S1,0);


#ifdef DEBUG
  Serial.begin(9600);
#endif


  getCodeByRobot();
} 



 
void loop() 
{  
  if (analogRead(A0) < 29 && analogRead(A1) < 29) { //езда прямо
    go(40, 0, 0);
    delay(10);
  }

  
  if (analogRead(A0) >= 29 && analogRead(A1) >= 29) { //остановка на линии, ожидание действия
    delay(450);
    go(0, 0, 0);
    delay(1500);

    // выбор действия по команде
    switch(colors[currentComand]){
      case B_RED:
        tunk180(0);
        break;
      case B_BLUE:
        tank90(0);
        break;
      case B_YELLOW:
        tank90(1);
        break;
      case B_EMPTY:
        go(50, 0, 0);
        break;
    }
    // к следующей команде
    currentComand++;
  }

  if (analogRead(A0) >= 29 && analogRead(A1) < 29) { //поворот налево
    goWheel(35, 15, 0, 0);
    delay(10);
  }

  if (analogRead(A1) >= 29 && analogRead(A1) < 29) { //поворот направо
    goWheel(15, 35, 0, 0);
    delay(10);
  }



// ----------------------- Считывание цвета ------------------------
int tempColors[4];//= {0,0,0,0};

int getCodeByRobot(){

  go(30, 0, 0);                                                 


  delay(1000); //задержка до первого кубика                    !!!

  
  for(int cubeI = 0; cubeI < 10; cubeI++){
    go(0, 0, 0);
    for(int i = 0; i < 4; i++){
      tempColors[i] = getColor();
      delay(5); // задержка между считываниями на кубике       
    }
    
    if(tempColors[0] == tempColors[1]){
      colors[cubeI] = tempColors[0];
    }else if(tempColors[1] == tempColors[2]){
      colors[cubeI] = tempColors[1];
    }else if(tempColors[0] == tempColors[2]){
      colors[cubeI] = tempColors[2];
    }else if(tempColors[0] != tempColors[1] && tempColors[1] != tempColors[2]) {
      colors[cubeI] = tempColors[3];
    }
    go(30, 0, 0);
    delay(150); // задержка до следующего куба                 !!!
  }
  
}

int getColor(){

  int red;
  int green;
  int blue;

  // установить R фильтр
  digitalWrite(PIN_S2,0);
  digitalWrite(PIN_S4,0);
  // Получение частоты на выходе
  red = pulseIn(PIN_Out, 0);

#ifdef DEBUG
  // вывод в последовательный порт
  Serial.print("R=");
  Serial.print(red);
  Serial.print("=");
  Serial.print(30 < red && red < 80);
#endif
  
  delay(80);                                      



  // установить G фильтр
  digitalWrite(PIN_S2,1);
  digitalWrite(PIN_S4,1);
  // Получение частоты на выходе
  green = pulseIn(PIN_Out, 0);

#ifdef DEBUG
  // вывод в последовательный порт
  Serial.print(" G=");
  Serial.print(green);
  Serial.print("=");
  Serial.print(40 < green && green < 70);
#endif
  
  delay(80);                                      
  


  // установить B фильтр
  digitalWrite(PIN_S2,0);
  digitalWrite(PIN