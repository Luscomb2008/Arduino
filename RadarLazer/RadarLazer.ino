#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Servo.h>

Servo myServo;

const int servoPin = 9;
const int trigPin = 10;
const int echoPin = 11;
const int joystickX = A0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long detectionThreshold = 50;

int currentAngle = 90; // Start at neutral midpoint

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
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

  // Calculate the speed: the further the joystick is pushed, the faster the movement
  int speed = map(abs(joyVal - 512), 0, 512, 1, 5);  // 1 to 5 (speed multiplier)

  // Smoothly move towards the target, adjusting speed dynamically
  if (abs(currentAngle - targetAngle) > 1) {
    if (currentAngle < targetAngle) {
      currentAngle += speed;  // Move towards target with variable speed
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

  delay(10); // Faster, smoother movement
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void handleDetection(long distance) {
  if (distance <= detectionThreshold) {
    flashOLED();
  } else {
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
