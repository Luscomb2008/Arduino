int blueled = 9;
int greenled = 10;
int clearled = 3;
int redled = 5;
int brightness = 0;
int fadeAmount = 5;

void setup() {
  pinMode(blueled, OUTPUT);
  pinMode(greenled, OUTPUT);
  pinMode(clearled, OUTPUT);
  pinMode(redled, OUTPUT);
}

void loop() {
  fadeLED(blueled);
  fadeLED(greenled);
  fadeLED(clearled);
  fadeLED(redled);
}

void fadeLED(int ledPin) {
  // Fade in
  for (brightness = 0; brightness <= 255; brightness += fadeAmount) {
    analogWrite(ledPin, brightness);
    delay(30);
  }

  // Fade out
  for (brightness = 255; brightness >= 0; brightness -= fadeAmount) {
    analogWrite(ledPin, brightness);
    delay(30);
  }
}
