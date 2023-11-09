#include <HX711.h>

//#include "HX711.h"
#include <LiquidCrystal.h>
#include <Servo.h>
LiquidCrystal lcd(4, 3, 5, 6, 7, 8); // Выходы к LCD (rs, en, d4, d5, d6, d7)
#define calibration_factor -440000.0 //Калибруем весы ,стоит для тензо 5кг
#define ESC_PIN 9                    // выход к регулю оборотов
#define POT_PIN A0                   // к резюку крутилка оборотов
#define BUZZER 2                     // пищит если ставим
#define CURRENT_SENSOR A2            // к датчику тока 
#define VOLTAGE_PIN A1               // к делителю напряжения 100кОм и 20кОм до 30вольт ,хотим в космос ,пересчитываем

HX711 scale;//(A4, A5);                // к ласам. к весам , короче туда
Servo esc;
void setup() 
{
  pinMode(BUZZER, OUTPUT);
  lcd.begin(20, 4);
  lcd.print("Thrust Stand 1");
  delay(1000);
  scale.set_scale(calibration_factor); 
  scale.tare();
  esc.attach(ESC_PIN);
  while(analogRead(POT_PIN) > 20)
  {
    lcd.print("TO START SET");
    lcd.setCursor(0,1);
    lcd.print("THROTTLE TO 0");
    delay(200);
    lcd.clear();
  }
}

    void loop() {

    float current=0.0;
    long int sum_of_reads = 0;

    for (int x = 0; x < 100; x++)
{
  sum_of_reads += analogRead(CURRENT_SENSOR);
  delay(2);
}
  delay(50);
  lcd.clear();
  current = sum_of_reads/100;
  current = (2.5 - (current * (5.0 / 1024.0)) )/0.066; // 0.066 для датчика тока 30А
  current = abs(current);
  current -= 0.07;
  current = abs(current);

  int escValue = map(analogRead(POT_PIN), 0, 1023, 900, 2000);
  esc.write(escValue);
  lcd.print("T:");
  lcd.print(abs(scale.get_units()));
  lcd.print("kg ");
  lcd.setCursor(11,0);
  lcd.print("C:");
  lcd.print(current);
  lcd.print("A");
  lcd.setCursor(0,1);
  lcd.print("E:");
  lcd.print(escValue);
  lcd.setCursor(11,1);
  lcd.print("P:");
  lcd.print(((analogRead(VOLTAGE_PIN)*31.0)/1024.0)*current); // крутим 31.0 если вольтаж врет 
  lcd.print("W");
  lcd.setCursor(0,2);
  lcd.print("V:");
  lcd.print((analogRead(VOLTAGE_PIN)*31.0)/1024.0);         // вбиваем 31 или что там сверху вышло при настройке вольтажа 
  lcd.setCursor(11,2);
  lcd.print("Ef:");
  lcd.print( 1000*(abs(scale.get_units()))/(((analogRead(VOLTAGE_PIN)*31.0)/1024.0)*current));      // вбиваем 31 или что там сверху вышло при настройке вольтажа
  

  
  //тут пищим
  if(escValue > 1500){
    if(digitalRead(BUZZER) == HIGH){
      digitalWrite(BUZZER, LOW);
    }else{
      digitalWrite(BUZZER, HIGH);
    }
  }else{
    digitalWrite(BUZZER, LOW);
  }

}
