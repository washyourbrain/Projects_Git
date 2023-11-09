
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(10, OUTPUT);
  pinMode(A0, INPUT);
}

// the loop function runs over and over again forever
void loop() {
  analogWrite(
    10, 
     map(
      analogRead(A0), 0, 1024, 100, 255
    )
  );
}