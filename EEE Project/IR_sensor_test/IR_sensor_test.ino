const int irSensorPin = 2;  // Connect the OUT pin of IR sensor to digital pin 2
const int ledPin = 13;      // Optional: built-in LED on pin 13

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int sensorValue = digitalRead(irSensorPin);

  if (sensorValue == LOW) {
    // Object detected (for many sensors, LOW means detection)
    Serial.println("Object Detected");
    digitalWrite(ledPin, HIGH);
  } else {
    // No object
    Serial.println("No Object");
    digitalWrite(ledPin, LOW);
  }

  delay(500);  // Delay for readability
}
