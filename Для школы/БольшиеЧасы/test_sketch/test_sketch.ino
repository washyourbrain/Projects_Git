
// === Константы ===================================================================================================

// выводы к сдвиговым регистрам часов
#define PIN_NUMBERS_LATCH 6
#define PIN_NUMBERS_CLOCK 5
#define PIN_NUMBERS_DATA_SECONDS 4
#define PIN_NUMBERS_DATA_MINUTES 7
#define PIN_NUMBERS_DATA_HOURS 8

uint8_t testNumbersArray[11] = {
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
  0b00001000 // "-"
};

// === Динамические переменные ===================================================================================================

uint8_t currentNumbers[6] = {0,1,2,3,4,5};

// === Начальная настройка ===================================================================================================

void setup(){

  // выводы отвечающие за отображение времени
  pinMode(PIN_NUMBERS_LATCH, 1);
  pinMode(PIN_NUMBERS_CLOCK, 1);
  pinMode(PIN_NUMBERS_DATA_SECONDS, 1);
  pinMode(PIN_NUMBERS_DATA_MINUTES, 1);
  pinMode(PIN_NUMBERS_DATA_HOURS, 1);

  

  outClockNumbers();

}


// === Главный цикл ===================================================================================================

void loop(){

  delay(1000);

  // инкремент всех чисел в currentNumbers по кругу
  for(uint8_t numberI = 0; numberI < sizeof(currentNumbers); numberI++){
    currentNumbers[numberI] = 
      (currentNumbers[numberI] == sizeof(testNumbersArray) - 1) ? (0) : (currentNumbers[numberI] + 1);
  }

  // вывод чисел на часы
  outClockNumbers();

}



// === Доп функции ===================================================================================================

// вывод 8-ми битов чисел в массиве currentNumbers
void outClockNumbers(){

  // вывод битов вторых цифр в парах
  for (uint8_t bitPos = 0; bitPos < 8; bitPos++)  {
    // отправляем один бит данных 
    digitalWrite(PIN_NUMBERS_DATA_HOURS,   (testNumbersArray[currentNumbers[1]] >> bitPos) & 1);
    digitalWrite(PIN_NUMBERS_DATA_MINUTES, (testNumbersArray[currentNumbers[3]] >> bitPos) & 1);
    digitalWrite(PIN_NUMBERS_DATA_SECONDS, (testNumbersArray[currentNumbers[5]] >> bitPos) & 1);
    
    // тактируем каждую отправку через clock
    digitalWrite(PIN_NUMBERS_CLOCK, HIGH);
    digitalWrite(PIN_NUMBERS_CLOCK, LOW);  
  }  
  
  // вывод битов первых цифр в парах (они смещают вторые пары на вторые позиции)
  for (uint8_t bitPos = 0; bitPos < 8; bitPos++)  {
    // отправляем один бит данных цифры
    digitalWrite(PIN_NUMBERS_DATA_HOURS,   (testNumbersArray[currentNumbers[0]] >> bitPos) & 1);
    digitalWrite(PIN_NUMBERS_DATA_MINUTES, (testNumbersArray[currentNumbers[2]] >> bitPos) & 1);
    digitalWrite(PIN_NUMBERS_DATA_SECONDS, (testNumbersArray[currentNumbers[4]] >> bitPos) & 1);
    
    // тактируем каждую отправку через clock
    digitalWrite(PIN_NUMBERS_CLOCK, HIGH);
    digitalWrite(PIN_NUMBERS_CLOCK, LOW);  
  }
  
  // выводим данные из внутренних байтов регистров на пины регистров 
  // щёлкая задвижкой
  digitalWrite(PIN_NUMBERS_LATCH, HIGH);  
  digitalWrite(PIN_NUMBERS_LATCH, LOW); 
}





