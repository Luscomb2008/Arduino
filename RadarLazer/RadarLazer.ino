#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Servo.h>

Servo myServo;

const int servoPin = 9;
const int trigPin = 10;
const int echoPin = 11;

// OLED display size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Reset pin is not used for I2C

// Create the I2C OLED display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Distance threshold in centimeters (e.g., 50 cm)
const long detectionThreshold = 50;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Set up the pins for the servo and ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(servoPin);

  // Initialize the I2C OLED display with I2C address 0x3C (common address)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Stay in an infinite loop if display doesn't initialize
  }

  display.display();  // Initial display setup
  delay(2000);  // Wait to allow the display to initialize

  display.clearDisplay();
}

void loop() {
  // Sweep from 0 to 180 degrees for the servo
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);  // Move the servo
    delay(15);  // Wait for the servo to move

    long distance = getDistance();  // Measure distance
    handleDetection(distance);  // Check if an object is detected

    Serial.print(pos);
    Serial.print(",");
    Serial.println(distance);
  }

  delay(500);

  // Sweep from 180 to 0 degrees for the servo
  for (int pos = 180; pos >= 0; pos--) {
    myServo.write(pos);  // Move the servo
    delay(15);  // Wait for the servo to move

    long distance = getDistance();  // Measure distance
    handleDetection(distance);  // Check if an object is detected

    Serial.print(pos);
    Serial.print(",");
    Serial.println(distance);
  }

  delay(500);
}

long getDistance() {
  // Send a pulse to the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the pulse duration from the echo pin
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance in centimeters (sound travels at 0.034 cm per microsecond)
  long distance = duration * 0.034 / 2;
  return distance;
}

void handleDetection(long distance) {
  if (distance <= detectionThreshold) {
    // If an object is detected within the threshold, flash the OLED
    flashOLED();
  } else {
    // Otherwise, show a "No detection" message on the OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);  // White text
    display.setCursor(10, 10);
    display.print(F("No detection"));
    display.display();
  }
}

void flashOLED() {
  long currentMillis = millis();
  static long lastFlashTime = 0;
  static bool isFlashing = false;

  // Flash the OLED every 500 milliseconds
  if (currentMillis - lastFlashTime >= 500) {
    lastFlashTime = currentMillis;

    if (isFlashing) {
      display.clearDisplay();  // Clear the screen (black background)
    } else {
      display.fillScreen(SSD1306_WHITE);  // White background
      display.setTextSize(2);
      display.setTextColor(SSD1306_BLACK);  // Black text
      display.setCursor(10, 20);
      display.print(F("Detected!"));
    }

    isFlashing = !isFlashing;
    display.display();
  }
}
