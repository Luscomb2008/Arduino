#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Servo.h>

Servo myServo;

const int servoPin = 9;
const int trigPin = 10;
const int echoPin = 11;
const int joystickX = A0;
const int buzzerPin = 8; // Passive Speaker connected to pin 8

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long detectionThreshold = 50; // in cm

int currentAngle = 90; // Start at neutral midpoint

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT); // Buzzer (speaker) pin setup

  myServo.attach(servoPin);
  myServo.write(currentAngle);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  int joyVal = analogRead(joystickX);
  int targetAngle = map(joyVal, 0, 1023, 0, 180);

  // Calculate the speed based on joystick position
  int speed = map(abs(joyVal - 512), 0, 512, 1, 5);

  // Smooth servo movement
  if (abs(currentAngle - targetAngle) > 1) {
    if (currentAngle < targetAngle) {
      currentAngle += speed;
      if (currentAngle > targetAngle) currentAngle = targetAngle;
    } else {
      currentAngle -= speed;
      if (currentAngle < targetAngle) currentAngle = targetAngle;
    }
    myServo.write(currentAngle);
  }

  long distance = getDistance();
  handleDetection(distance);

  Serial.print("Joystick: ");
  Serial.print(joyVal);
  Serial.print(" | Target: ");
  Serial.print(targetAngle);
  Serial.print(" | Angle: ");
  Serial.print(currentAngle);
  Serial.print(" | Distance: ");
  Serial.println(distance);

  delay(10);
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
    soundAlarm(); // Play alarm sound when object is detected
  } else {
    noTone(buzzerPin); // Stop the alarm
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.print(F("No detection"));
    display.display();
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

// Function to play an alarm-like sound on the speaker
void soundAlarm() {
  // Alternate between high and low frequency to simulate an alarm sound
  tone(buzzerPin, 1000);  // 1 kHz (high-pitched tone)
  delay(200);             // Wait for a short duration

  tone(buzzerPin, 400);  // 400 Hz (low-pitched tone)
  delay(200);             // Wait for a short duration

  tone(buzzerPin, 1000);  // Repeat with the high-pitched tone
  delay(200);             // Wait for a short duration

  tone(buzzerPin, 400);  // Repeat with the low-pitched tone
  delay(200);             // Wait for a short duration
}
