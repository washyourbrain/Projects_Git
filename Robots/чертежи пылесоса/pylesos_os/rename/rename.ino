
// подключение модуля bluetooth
#include <SoftwareSerial.h>
#define BT_TX 8
#define BT_RX 7
SoftwareSerial BTserial(BT_TX, BT_RX);  // TX, RX

void setup() {

  // инициализация bluetooth
  BTserial.begin(9600);
  Serial.begin(9600);


  // ждем команды
  Serial.println("print to start");
  while (! (Serial.available() > 0));
  Serial.println("Начали");
  delay("1000");

  BTserial.print("AT"); Serial.println("AT");
  delay("1000");
  readBack();  // читаем ответ модуля

  BTserial.print("AT + NAME:PYLESOS"); Serial.println("AT + NAME:PYLESOS");
  delay("1000");
  readBack();  // читаем ответ модуля
  
  BTserial.print("AT + PIN1234"); Serial.println("AT + PIN1234");
  delay("1000");
  readBack();  // читаем ответ модуля


  delay("1000");
  Serial.println("Прошейте другой скетч либо отключите модуль");

}

void loop() {
}



void readBack() {
  while (BTserial.available() > 0) {         // если в буфере есть данные
    char incomingChar = BTserial.read();  // читаем из буфера
    Serial.println(incomingChar);
  }
}

