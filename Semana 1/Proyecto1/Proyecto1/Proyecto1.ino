// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  for(int i = 7; i>0;i--){
    if(i>=3){
      digitalWrite(10, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(1000);                      // wait for a second
      digitalWrite(10, LOW);   // turn the LED off by making the voltage LOW
      delay(2000);    
    }else{
      digitalWrite(11, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(500);                      // wait for a second
      digitalWrite(11, LOW);   // turn the LED off by making the voltage LOW
      delay(1000);    
    }
    
  }
}