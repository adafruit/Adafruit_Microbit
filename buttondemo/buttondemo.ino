void setup() {  
  Serial.begin(9600);
  
  Serial.println("microbit is ready!");
  
  pinMode(PIN_BUTTON_A, INPUT);
  pinMode(PIN_BUTTON_B, INPUT);
}

void loop(){
  if (! digitalRead(PIN_BUTTON_A)) {
    Serial.println("Button A pressed");
  }
  if (! digitalRead(PIN_BUTTON_B)) {
    Serial.println("Button B pressed");
  }
  delay(10);
}
