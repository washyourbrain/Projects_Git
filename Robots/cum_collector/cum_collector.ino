

void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);//pwm
  pinMode(1, INPUT);// r
  pinMode(0, INPUT);// bttn
  digitalWrite(0, HIGH);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(!digitalRead(0)){
    analogWrite(11,analogRead(1));
  }else
    analogWrite(11,0);
}
