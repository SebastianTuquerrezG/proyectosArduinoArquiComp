#define LED_GREEN 10
#define LED_BLUE 12
#define LED_RED 11
int varAux = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  if(varAux > 120){
    digitalWrite(LED_RED, HIGH);  // turn the LED on (HIGH is the voltage level)  
  }else if(varAux < 50){
    digitalWrite(LED_BLUE, HIGH);  // turn the LED on (HIGH is the voltage level)  
  }else{
    digitalWrite(LED_GREEN, HIGH);  // turn the LED on (HIGH is the voltage level)
    varAux = 0;
  } 
  varAux++; 
    delay(500);         
    digitalWrite(LED_RED, LOW); 
    digitalWrite(LED_BLUE, LOW); 
    digitalWrite(LED_GREEN, LOW);
      
}