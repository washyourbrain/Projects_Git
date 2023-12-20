

void setup() {
  // подключение кнопки подтянутой другим концом на землю
  pinMode(7, INPUT);
  digitalWrite(7, 1);

  // подключение светодиода
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(8, 0);
  digitalWrite(9, 0);
  digitalWrite(10, 0);
}

// текущее состояние светодиода
int8_t ledState = 0;
// предыдущее состояние кнопки (для подавления дребезга)
bool previousButtonState = false;


void loop() {

  // проверяем нажата ли кнопка (и до этого она была отпущена)
  if(!digitalRead(7) && !previousButtonState){
    // ждем чтобы прошел дребезг контактов
    delay(10);
    // проверяем еще раз нажата ли кнопка
    if(!digitalRead(7)){
      previousButtonState = true;
      ledState = (ledState + 1) % 4; // переключение ledState -> 0, 1, 2, 3, 0, 1, 2, 3...

      // переключение светодиодов
      digitalWrite(8, (ledState == 1));
      digitalWrite(9, (ledState == 2));
      digitalWrite(10, (ledState == 3));
    }

  }

  // запоминаем предыдущее значение кнопки
  previousButtonState = !digitalRead(7); 
}
