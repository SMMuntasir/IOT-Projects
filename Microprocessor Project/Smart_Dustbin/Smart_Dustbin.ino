#define BLYNK_TEMPLATE_ID "TMPL6xp9rQaCC"
#define BLYNK_TEMPLATE_NAME "Smart Dustbin"
#define BLYNK_AUTH_TOKEN "LTO6g-4JxAwUNUHdLgOCvw_e1JhwhWRG"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "DESKTOP-2415SD9 6742";
char pass[] = "C231037M";

BlynkTimer timer;
Servo servo;

// Configurable Servo Pins and Angles
#define SERVO_PIN 13
const int lidOpenAngle = 150;
const int lidCloseAngle = 0;

// Pins for sensors and indicators
#define trigPinLid 25
#define echoPinLid 26
#define trigPinBin 33
#define echoPinBin 32
#define redLED 12
#define greenLED 14
#define buzzer 27

bool manualOpen = false;
unsigned long lastBuzzerTime = 0;
unsigned long lidOpenedTime = 0;
bool lidOpen = false;

void setup() {
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);

  // Servo
  servo.attach(SERVO_PIN);
  servo.write(lidCloseAngle);

  // Sensors
  pinMode(trigPinLid, OUTPUT);
  pinMode(echoPinLid, INPUT);
  pinMode(trigPinBin, OUTPUT);
  pinMode(echoPinBin, INPUT);

  // LED & Buzzer
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, HIGH);
  digitalWrite(buzzer, LOW);

  // Run sensor every second
  timer.setInterval(1000L, readSensors);
}

void loop() {
  Blynk.run();
  timer.run();
  checkAutoCloseLid();
}

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout after 30ms
  int distance = duration * 0.034 / 2;
  return (distance == 0) ? 999 : distance;
}

void readSensors() {
  int personDistance = getDistance(trigPinLid, echoPinLid);
  delay(50);
  int binDistance = getDistance(trigPinBin, echoPinBin);

  Serial.print("Person Distance: ");
  Serial.print(personDistance);
  Serial.print(" cm | Bin Distance: ");
  Serial.println(binDistance);

  int binLevel = map(binDistance, 16, 3, 0, 100);
  binLevel = constrain(binLevel, 0, 100);

  Blynk.virtualWrite(V0, binLevel);

  // LED indicator
  if (binLevel > 80) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);

    if (millis() - lastBuzzerTime >= 5000) {
      digitalWrite(buzzer, HIGH);
      delay(300);
      digitalWrite(buzzer, LOW);
      lastBuzzerTime = millis();
    }
  } else {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(buzzer, LOW);
  }

  // Automatic lid control
  if (!manualOpen) {
    if (personDistance > 0 && personDistance <= 10 && !lidOpen) {
      servo.write(lidOpenAngle);
      lidOpenedTime = millis();
      lidOpen = true;
      Blynk.virtualWrite(V1, 1);
    }
  }
}

void checkAutoCloseLid() {
  if (!manualOpen && lidOpen && (millis() - lidOpenedTime > 2000)) {
    servo.write(lidCloseAngle);
    lidOpen = false;
    Blynk.virtualWrite(V1, 0);
  }
}

// Manual control from Blynk (V1)
BLYNK_WRITE(V1) {
  int value = param.asInt();
  if (value == 1) {
    servo.write(lidOpenAngle);
    manualOpen = true;
    lidOpen = true;
    Blynk.virtualWrite(V1, 1);
  } else {
    servo.write(lidCloseAngle);
    manualOpen = false;
    lidOpen = false;
    Blynk.virtualWrite(V1, 0);
  }
}
