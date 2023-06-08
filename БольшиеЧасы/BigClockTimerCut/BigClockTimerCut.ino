// === Библиотеки ==================================================================================================
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"


// === Настраиваемые константы =====================================================================================


// время уроков
uint8_t lessonsTime [7][8] = {
  {0,8,3,0, 0,9,1,5},
  {0,9,2,5, 1,0,1,0},
  {1,0,3,0, 1,1,1,5},
  {1,1,3,5, 1,2,2,0},
  {1,2,3,5, 1,3,2,0},
  {1,3,3,0, 1,4,1,5},
  {1,4,2,5, 1,5,1,0}
};


// раз в сколько милисекунд обновлять значение на индикаторах
uint16_t updateIndicatorTime = 1000;
// раз в сколько милисекунд получать значение с таймера
uint16_t updateRTCTime = 5000;
// раз в сколько милисекунд   название говорит само за себя
uint16_t updateLohTime = 45000;


// выводы к сдвиговым регистрам часов
#define PIN_NUMBERS_LATCH 6
#define PIN_NUMBERS_CLOCK 5
#define PIN_NUMBERS_DATA_SECONDS 4
#define PIN_NUMBERS_DATA_MINUTES 7
#define PIN_NUMBERS_DATA_HOURS 8


/*
+--3--+
|     |
4     2
|     |
+--5--+
|     |
8     6
|     |
+--7--+
0b12345678
*/  
uint8_t numbersCodes[13] = {
  0b01110111, // 0
  0b01000100, // 1
  0b01101011, // 2
  0b01101110, // 3
  0b01011100, // 4
  0b00111110, // 5
  0b00111111, // 6
  0b01100100, // 7
  0b01111111, // 8
  0b01111110, // 9
  0b00001000, // "-"
  0b00010011, // "L"  
  0b00011101  // "h"
};

// === Динамические переменные ===================================================================================================

// переменные счетчиков
uint64_t updateIndicatorCounter = 0; // unsigned long
uint64_t updateRTCCounter = 0;
uint16_t updateLohCounter = 0;


// текущее значение на индикаторах
uint8_t currentNumbers[6] = {2,3,5,9,4,5};

// подключенный таймер
RTC_DS3231 rtc;

// === Начальная настройка ===================================================================================================

void setup(){

  Serial.begin(57600);

  rtc.begin();
  // //проверка подключения часов реального времени (для отладки)
  // Serial.begin(57600);
  // while (!Serial); // wait for serial port to connect. Needed for native USB
  // if (! rtc.begin()) {
  //   Serial.println("Couldn't find RTC");
  //   Serial.flush();
  //   while (1) delay(10);
  // }
  

  // при первой прошивке загружаем в таймер реального времени время компиляции прошивки
  if (rtc.lostPower()|| true) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
    Serial.print(F(__DATE__));
    Serial.print(" ");
    Serial.print(F(__TIME__));
    Serial.print(" ");
    //Serial.println(nowTime.second());

    
    //Serial.print(nowTime.hour(), DEC);
    Serial.print(" ");
    //Serial.print(nowTime.minute(), DEC);
    Serial.print(" ");
    //Serial.println(nowTime.second(), DEC);
    delay(2000);
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


  // // выводы отвечающие за отображение времени
  // pinMode(PIN_NUMBERS_LATCH, 1);
  // pinMode(PIN_NUMBERS_CLOCK, 1);
  // pinMode(PIN_NUMBERS_DATA_SECONDS, 1);
  // pinMode(PIN_NUMBERS_DATA_MINUTES, 1);
  // pinMode(PIN_NUMBERS_DATA_HOURS, 1);

  
  // сброс таймеров
  updateIndicatorCounter = 0;
  updateRTCCounter = 0;

}


// === Главный цикл ===================================================================================================

void loop(){

  // таймер получения времени с часов реального времени
  if(millis() - updateRTCCounter >= updateRTCTime){
    updateRTCCounter = millis();

    // запрашиваем время из RTC
    DateTime testTime = rtc.now();
    /*
    
    
    DateTime now = rtc.now();
    uint8_t currentNumber = now.hour() % 10;
    Serial.print("pppp:");
    Serial.print(currentNumber);
    */

    // вывод данных в поля
    
    // Serial.print(testTime.year(), DEC);
    // Serial.print('/');
    // Serial.print(testTime.month(), DEC);
    // Serial.print('/');
    // Serial.print(testTime.day(), DEC);
    // Serial.print(" (");
    testTime.dayOfTheWeek();
    // Serial.print(") ");


    Serial.print(testTime.hour(), DEC);
    Serial.print(" ");
    Serial.print(testTime.minute(), DEC);
    Serial.print(" ");
    Serial.println(testTime.second(), DEC);

    currentNumbers[0] = testTime.hour() / 10;
    currentNumbers[1] = testTime.hour() % 10;
    currentNumbers[2] = testTime.minute() / 10;
    currentNumbers[3] = testTime.minute() % 10;
    currentNumbers[4] = testTime.second() / 10;
    currentNumbers[5] = testTime.second() % 10;

  }

  // // таймер обновления индикаторов
  // if(millis() - updateIndicatorCounter >= updateIndicatorTime){
  //   updateIndicatorCounter = millis();
  //   
  //   // // инкремент текущего времени -------------------------------------------------- todo проверить ---------------------
  //   // // секунды
  //   // if(currentNumbers[5] != 9){
  //   //   currentNumbers[5]++;
  //   // }else{
  //   //   currentNumbers[5]=0;

  //   //   // десятки секунд
  //   //   if(currentNumbers[4] != 5){
  //   //     currentNumbers[4]++;
  //   //   }else{
  //   //     currentNumbers[4]=0;

  //   //     // минуты
  //   //     if(currentNumbers[3] != 9){
  //   //       currentNumbers[3]++;
  //   //     }else{
  //   //       currentNumbers[3]=0;

  //   //       // десятки минут
  //   //       if(currentNumbers[2] != 5){
  //   //         currentNumbers[2]++;
  //   //       }else{
  //   //         currentNumbers[2]=0;

  //   //         // инкремент часов
  //   //         if(currentNumbers[0] == 2 && currentNumbers[1] == 3){
  //   //           currentNumbers[0] = 0;
  //   //           currentNumbers[1] = 0;
  //   //         }else if(currentNumbers[1] == 9){
  //   //           currentNumbers[0]++;
  //   //           currentNumbers[1] = 0;
  //   //         }else
  //   //          currentNumbers[1]++;
            
  //   //       }
  //   //     }
  //   //   }
  //   // }

  //   // Serial.print(currentNumbers[0]);
  //   // Serial.print(currentNumbers[1]);
  //   // Serial.print(":");
  //   // Serial.print(currentNumbers[2]);
  //   // Serial.print(currentNumbers[3]);
  //   // Serial.print(":");
  //   // Serial.print(currentNumbers[4]);
  //   // Serial.println(currentNumbers[5]);

  //   // выводить ли обратный отсчет на уроке
  //   // вывод чисел на часы
  //   outClockNumbers(
  //     currentNumbers[0],
  //     currentNumbers[1],
  //     currentNumbers[2],
  //     currentNumbers[3],
  //     currentNumbers[4],
  //     currentNumbers[5]
  //   );
  // }
}



















