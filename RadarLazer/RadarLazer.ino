#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

Servo myServo;

const int servoPin = 9;
const int trigPin = 10;
const int echoPin = 11;
const int joystickX = A0;
const int buzzerPin = 4;  // Passive Buzzer pin
const int ledPin = 7;     // LED strip data pin (connected to digital 7)
const int numPixels = 30; // Number of LEDs in the strip (change this number as needed)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_NeoPixel strip(numPixels, ledPin, NEO_GRB + NEO_KHZ800); // Initialize the WS2812 strip

const long detectionThreshold = 50; // in cm

int currentAngle = 90; // Start at neutral midpoint
unsigned long lastSirenTime = 0;  // Timer for siren effect
int sirenFrequency = 1000;         // Starting frequency for siren
int sirenDirection = 1;            // Direction of frequency change (+1 or -1)

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT); // Buzzer (speaker) pin setup

  myServo.attach(servoPin);  // Attach the servo to the correct pin
  myServo.write(currentAngle); // Start at neutral position

  // Initialize the NeoPixel strip
  strip.begin();
  strip.show();  // Initialize all pixels to off

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  int joyVal = analogRead(joystickX);  // Read joystick X-axis value
  int targetAngle = map(joyVal, 0, 1023, 0, 180); // Map joystick value to angle (0 to 180)

  // Calculate the speed based on joystick position
  int speed = map(abs(joyVal - 512), 0, 512, 1, 5);

  // Smooth servo movement based on joystick input
  if (abs(currentAngle - targetAngle) > 1) {
    if (currentAngle < targetAngle) {
      currentAngle += speed;
      if (currentAngle > targetAngle) currentAngle = targetAngle;
    } else {
      currentAngle -= speed;
      if (currentAngle < targetAngle) currentAngle = targetAngle;
    }
    myServo.write(currentAngle); // Move the servo to the new angle
  }

  long distance = getDistance();  // Get the distance from the ultrasonic sensor
  handleDetection(distance);      // Handle the detection logic

  // Debugging prints for servo and distance
  Serial.print("Joystick: ");
  Serial.print(joyVal);
  Serial.print(" | Target: ");
  Serial.print(targetAngle);
  Serial.print(" | Angle: ");
  Serial.print(currentAngle);
  Serial.print(" | Distance: ");
  Serial.println(distance);
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; // Convert to cm
}

void handleDetection(long distance) {
  if (distance <= detectionThreshold && distance > 0) {
    flashOLED();
    playSiren();    // Play siren sound when object is detected
    flashLEDs();    // Flash Red and Blue LEDs when detection occurs
  } else {
    noTone(buzzerPin);  // Stop the siren sound
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.print(F("No detection"));
    display.display();

    // Turn off the LED strip
    turnOffLEDs();
  }
}

void flashOLED() {
  long currentMillis = millis();
  static long lastFlashTime = 0;
  static bool isFlashing = false;

  if (currentMillis - lastFlashTime >= 500) {
    lastFlashTime = currentMillis;

    if (isFlashing) {
      display.clearDisplay();
    } else {
      display.fillScreen(SSD1306_WHITE);
      display.setTextSize(2);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(10, 20);
      display.print(F("Detected!"));
    }

    isFlashing = !isFlashing;
    display.display();
  }
}

// Function to play a siren-like sound on the passive buzzer
void playSiren() {
  unsigned long currentMillis = millis();
  
  // Make the siren change pitch smoothly
  if (currentMillis - lastSirenTime >= 50) {  // Update frequency every 50ms
    lastSirenTime = currentMillis;
    
    // Adjust frequency based on the direction (increase or decrease)
    tone(buzzerPin, sirenFrequency);

    // Change frequency for next iteration
    sirenFrequency += sirenDirection * 10;

    // Reverse direction if frequency goes out of desired range (e.g., between 500Hz and 1500Hz)
    if (sirenFrequency <= 500 || sirenFrequency >= 1500) {
      sirenDirection = -sirenDirection;  // Change direction
    }
  }
}

// Flash Red and Blue LEDs on the WS2812 strip when an object is detected
void flashLEDs() {
  static long lastLEDFlash = 0;
  long currentMillis = millis();

  // Alternate flashing every 500ms
  if (currentMillis - lastLEDFlash >= 500) {
    lastLEDFlash = currentMillis;

    // Alternate between Red and Blue colors
    static bool isRed = true;
    if (isRed) {
      // Flash Red
      setLEDColor(255, 0, 0);  // Red
    } else {
      // Flash Blue
      setLEDColor(0, 0, 255);  // Blue
    }
    isRed = !isRed;
  }
}

// Set the entire WS2812 strip to a single color
void setLEDColor(int red, int green, int blue) {
  for (int i = 0; i < numPixels; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue)); // Set color for each LED
  }
  strip.show();  // Update the strip
}

// Turn off all LEDs on the WS2812 strip
void turnOffLEDs() {
  for (int i = 0; i < numPixels; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Turn off each LED
  }
  strip.show();  // Update the strip
}
