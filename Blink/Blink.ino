int blueLED = 12;
int redLED = 11;   // Pin for red LED
int greenLED = 13;  // Pin for green LED
int clearLED = 8;   // Pin for clear LED

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pins as outputs
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);  // Initialize green LED pin
  pinMode(clearLED, OUTPUT);  // Initialize clear LED pin
}

// the loop function runs over and over again forever
void loop() {
  // Blue LED on and off
  digitalWrite(blueLED, HIGH);  // turn the blue LED on
  delay(500);                    // wait for 0.5 seconds
  digitalWrite(blueLED, LOW);   // turn the blue LED off
  delay(500);                    // wait for 0.5 seconds
  
  // Red LED on and off
  digitalWrite(redLED, HIGH);   // turn the red LED on
  delay(500);                    // wait for 0.5 seconds
  digitalWrite(redLED, LOW);    // turn the red LED off
  delay(500);                    // wait for 0.5 seconds
  
  // Green LED on and off
  digitalWrite(greenLED, HIGH); // turn the green LED on
  delay(500);                    // wait for 0.5 seconds
  digitalWrite(greenLED, LOW);  // turn the green LED off
  delay(500);                    // wait for 0.5 seconds

  // Clear LED on and off
  digitalWrite(clearLED, HIGH); // turn the clear LED on
  delay(500);                    // wait for 0.5 seconds
  digitalWrite(clearLED, LOW);  // turn the clear LED off
  delay(500);                    // wait for 0.5 seconds
}
