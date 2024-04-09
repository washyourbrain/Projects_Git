





// дефайны настроек, прописываются перед подключением либы
#define TLED_ORDER ORDER_GRB   // порядок цветов
#define TLED_CHIP LED_WS2812   // чип светодиода ленты

/*
  Доступные модели лент и их порядок цветов
  Китайцы могут продавать копию с другим порядком =)
  LED_WS2811 // ORDER_GBR
  LED_WS2812 // ORDER_GRB
  LED_WS2813 // ORDER_GRB
  LED_WS2815 // ORDER_GRB
  LED_WS2818 // ORDER_RGB
  LED_WS6812 // ORDER_RGB + W
  LED_APA102 // ORDER_BGR
  LED_APA102_SPI // ORDER_BGR
*/

// ===== 1 пин ленты =====
// по умолч. PORTB
// можно указать свой порт, если у тиньки больше 8 ног или это вообще атмега
// пин указывается при создании
// #define TLED_PORT PORTB
// #define TLED_DDR DDRB

// ===== 2 пин SPI ленты =====
// программный SPI, например LED_APA102
// по умолч. PORTB
// можно указать свой порт, если у тиньки больше 8 ног или это вообще атмега
// пин указывается при создании
// #define TLED_CLK_PORT PORTB
// #define TLED_CLK_DDR DDRB
// #define TLED_DAT_PORT PORTB
// #define TLED_DAT_DDR DDRB

// ===== Запрет прерываний =====
// можно указать приоритет прерываний (умолч CLI_LOW):
// #define TLED_CLI CLI_OFF  // прерывания не запрещаются
// #define TLED_CLI CLI_LOW  // запрещаются на время передачи байта
// #define TLED_CLI CLI_AVER // запрещаются на время передачи светодиода (3 байта)
// #define TLED_CLI CLI_HIGH // запрещаются на время передачи всей ленты

// ===== Яркость =====
// #define TLED_STATIC_BRIGHT  // отключает изменение общей яркости вывода и экономит 60 байт флэш

// ===== SPI лента =====
// только при выборе LED_APA102_SPI
// пины не указываются в скетче
// #define TLED_SPI_CLOCK 8000000	// скорость аппаратного SPI для ленты LED_APA102_SPI

// либа
#include "tinyLED.h"
tinyLED<0> strip;       // указываем пин (в порядке порта)
// tinyLED<3, 4> strip; // <DATA, CLOCK> для 2-пин лент (APA102)
// tinyLED strip;       // для LED_APA102_SPI не указываем пины ВООБЩЕ!

#define NUMLEDS 50 // количество светодиодов (для циклов)

void setup() {
  // begin() - начать отправку (НУЖНО ТОЛЬКО ДЛЯ SPI ЛЕНТ ИЛИ CLI_HIGH)
  // end() - закончить отправку (НУЖНО ТОЛЬКО ДЛЯ SPI ЛЕНТ ИЛИ CLI_HIGH)
  // write() - отправить один байт
  // sendRGB(r,g,b) - отправить цвет на 1 диод
  // sendRGBW(r,g,b,w) - отправить на rgbw ленту (например WS6812)
  // send(mData) - отправить цвет в формате mData
  // sendBuffer(mData буфер, размер) - отправить внешний буфер на ленту
  // clear(размер) - очистить на длину
  // setBrightness(0-255) - яркость для всего вывода


  strip.begin();
  // for (byte i = 0; i < 20; i++) {
  //   strip.send(mRed);
  //   //strip.sendRGB(100,0,0);
  //   //strip.sendRGB(0,100,0);
  //   strip.send(mGreen);
  //   //strip.sendRGB(0,0,100);
  //   strip.send(mBlue);
  // }
  
  // delay(2000);
}

void loop() {
  // =================================================
  // иииии бегущая радуга!
  static byte count;
  count++;
  //strip.begin();
  for (int i = 0; i < NUMLEDS; i++) {
    strip.send(mWheel8(count + i * 255 / NUMLEDS)); // выводим радугу
  }
  //strip.end();
  delay(30);
}
