// === Библиотеки ==================================================================================================
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"


// === Настраиваемые константы =====================================================================================

// выводить ли обратный отсчет на уроках
bool outCutdown = false;

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
uint16_t updateRTCTime = 15000;
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
uint8_t currentNumbers[6] = {0,1,2,3,4,5};

// подключенный таймер
RTC_DS3231 rtc;
// обновляемая переменная с временем из таймера
DateTime nowTime;

// === Начальная настройка ===================================================================================================

void setup(){

  /*
  проверка подключения часов реального времени (для отладки)
  Serial.begin(57600);
  while (!Serial); // wait for serial port to connect. Needed for native USB
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  */

  // при первой прошивке загружаем в таймер реального времени время компиляции прошивки
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


  // выводы отвечающие за отображение времени
  pinMode(PIN_NUMBERS_LATCH, 1);
  pinMode(PIN_NUMBERS_CLOCK, 1);
  pinMode(PIN_NUMBERS_DATA_SECONDS, 1);
  pinMode(PIN_NUMBERS_DATA_MINUTES, 1);
  pinMode(PIN_NUMBERS_DATA_HOURS, 1);

  

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
    nowTime = rtc.now();

    // вывод данных в поля
    currentNumbers[0] = nowTime.hour() / 10;
    currentNumbers[1] = nowTime.hour() % 10;
    currentNumbers[2] = nowTime.minute() / 10;
    currentNumbers[3] = nowTime.minute() % 10;
    currentNumbers[4] = nowTime.second() / 10;
    currentNumbers[5] = nowTime.second() % 10;

  }

  // таймер обновления индикаторов
  if(millis() - updateIndicatorCounter >= updateIndicatorTime){
    updateIndicatorCounter = millis();



    // инкремент текущего времени -------------------------------------------------- todo проверить ---------------------
    // секунды
    if(currentNumbers[5] != 9){
      currentNumbers[5]++;
    }else{
      currentNumbers[5]=0;

      // десятки секунд
      if(currentNumbers[4] != 5){
        currentNumbers[4]++;
      }else{
        currentNumbers[4]=0;

        // минуты
        if(currentNumbers[3] != 9){
          currentNumbers[3]++;
        }else{
          currentNumbers[3]=0;

          // десятки минут
          if(currentNumbers[2] != 6){
            currentNumbers[2]++;
          }else{
            currentNumbers[2]=0;

            // инкремент часов
            if(currentNumbers[0] == 2 && currentNumbers[1] == 4){
              currentNumbers[0] = 0;
              currentNumbers[1] = 0;
            }else if(currentNumbers[1] == 9){
              currentNumbers[0]++;
              currentNumbers[1] = 0;
            }else
             currentNumbers[1]++;
            
          }
        }
      }
    }


    // выводить ли обратный отсчет на уроке
    if(outCutdown){
      
      // если это будет слишком медленно, можно будет завести переменную которая будет содержать
      // номер текущего урока, и будет его обсчитывать в updateRTCTime

      for(uint8_t lessonI = 0; lessonI < sizeof(lessonsTime) >> 3; lessonI++)
        if(
		      currentNumbers[0] >= lessonsTime[lessonI][0] &&
          currentNumbers[1] >= lessonsTime[lessonI][1] &&
          currentNumbers[2] >= lessonsTime[lessonI][2] &&
          currentNumbers[3] >= lessonsTime[lessonI][3] &&
		  
		      currentNumbers[0] <= lessonsTime[lessonI][4] &&
          currentNumbers[1] <= lessonsTime[lessonI][5] &&
          currentNumbers[2] <= lessonsTime[lessonI][6] &&
          currentNumbers[3] <= lessonsTime[lessonI][7] &&
          currentNumbers[4] <= 5
        ){

          int8_t outCutdownMinuteOnes = lessonsTime[lessonI][7] - 1 - currentNumbers[3];
          int8_t outCutdownMinuteTens = lessonsTime[lessonI][6] - currentNumbers[2];
          if(outCutdownMinuteOnes < 0){
            outCutdownMinuteOnes = 0;
            outCutdownMinuteTens--;
          }

          outClockNumbers(
            10, // "минус"
            0,
            outCutdownMinuteTens,
            outCutdownMinuteOnes,
            5 - currentNumbers[4],
            9 - currentNumbers[5]
          );

          // прерываем цикл, тк нашли текущий урок и вывели его время
          break;
        }

   

    }else{

      // вывод чисел на часы
      outClockNumbers(
        currentNumbers[0],
        currentNumbers[1],
        currentNumbers[2],
        currentNumbers[3],
        currentNumbers[4],
        currentNumbers[5]
      );
    }

  }

  // Лохкаунтер 
  if(millis() - updateLohCounter >= updateLohTime){
    updateLohCounter = millis();

    outClockNumbers(
      11,//"L"
      0,
      12,//"h"
      11,//"L"
      0,
      12//"h"
    );
  }

       
          outClockNumbers(
            10, // "минус"
            0,
            1,
            2,
            3,
            4
          );
          delay(100);
  


  // // инкремент всех чисел в currentNumbers по кругу
  // for(uint8_t numberI = 0; numberI < sizeof(currentNumbers); numberI++){
  //   currentNumbers[numberI] = 
  //     (currentNumbers[numberI] == sizeof(numbersCodes) - 1) ? (0) : (currentNumbers[numberI] + 1);
  // }
}



// === Доп функции ===================================================================================================

// вывод 8-ми битов чисел в массиве currentNumbers
void outClockNumbers(uint8_t symbol0, uint8_t symbol1, uint8_t symbol2, uint8_t symbol3, uint8_t symbol4, uint8_t symbol5){

  // вывод битов вторых цифр в парах
  for (uint8_t bitPos = 0; bitPos < 8; bitPos++)  {
    // отправляем один бит данных 
    digitalWrite(PIN_NUMBERS_DATA_HOURS,   (numbersCodes[symbol1] >> bitPos) & 1);
    digitalWrite(PIN_NUMBERS_DATA_MINUTES, (numbersCodes[symbol3] >> bitPos) & 1);
    digitalWrite(PIN_NUMBERS_DATA_SECONDS, (numbersCodes[symbol5] >> bitPos) & 1);
    
    // тактируем каждую отправку через clock
    digitalWrite(PIN_NUMBERS_CLOCK, HIGH);
    digitalWrite(PIN_NUMBERS_CLOCK, LOW);  
  }  
  
  // вывод битов первых цифр в парах (они смещают вторые пары на вторые позиции)
  for (uint8_t bitPos = 0; bitPos < 8; bitPos++)  {
    // отправляем один бит данных цифры
    digitalWrite(PIN_NUMBERS_DATA_HOURS,   (numbersCodes[symbol0] >> bitPos) & 1);
    digitalWrite(PIN_NUMBERS_DATA_MINUTES, (numbersCodes[symbol2] >> bitPos) & 1);
    digitalWrite(PIN_NUMBERS_DATA_SECONDS, (numbersCodes[symbol4] >> bitPos) & 1);
    
    // тактируем каждую отправку через clock
    digitalWrite(PIN_NUMBERS_CLOCK, HIGH);
    digitalWrite(PIN_NUMBERS_CLOCK, LOW);  
  }
  
  // выводим данные из внутренних байтов регистров на пины регистров 
  // щёлкая задвижкой
  digitalWrite(PIN_NUMBERS_LATCH, HIGH);  
  digitalWrite(PIN_NUMBERS_LATCH, LOW); 
}




























//==================================================================================







/*char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void loop () {
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    // calculate a date which is 7 days, 12 hours, 30 minutes, 6 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    Serial.print(" now + 7d + 12h + 30m + 6s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    delay(3000);
}
*/

