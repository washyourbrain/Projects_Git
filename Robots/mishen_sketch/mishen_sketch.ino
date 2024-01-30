#include "Adafruit_NeoPixel.h"
#define LED_COUNT 7
#define LED_PIN 7
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  pinMode(4, INPUT);
  digitalWrite (4, HIGH);
  pinMode(5, INPUT);
  digitalWrite (5, HIGH);
  pinMode(6, INPUT);
  digitalWrite (6, HIGH);

  strip.begin(); //Инициализируем ленту.
  Serial.begin(9600);
}


int pressedButton = -1;
int currSetate = -1;

void loop() {
  
  if (!digitalRead (4)) {
    pressedButton = 0;
    currSetate = 0;
  } else if  (!digitalRead (5)) {
    pressedButton = 1;
    currSetate = 0;
  } else if (!digitalRead (6)) {
    pressedButton = 2;
    currSetate = 0;
  }

  Serial.println(pressedButton);

  
  if(currSetate !=-1){
    frame(currSetate, pressedButton);
    currSetate++;
    if(currSetate == 7){
      currSetate = -1;
      for (int i = 0; i < LED_COUNT; i++){
        strip.setPixelColor(i, strip.Color(0, 0, 0)); // null
      }
      strip.show();
    }
    delay(30);
  }
    
}

void frame(int currSetate, int pressedButton) {
  strip.setPixelColor(currSetate, strip.Color(
    (pressedButton == 0)?(255):(0),
    (pressedButton == 1)?(255):(0),
    (pressedButton == 2)?(255):(0)
    ));
  strip.show();
}
