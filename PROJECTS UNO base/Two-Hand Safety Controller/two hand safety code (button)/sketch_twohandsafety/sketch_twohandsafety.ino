// ======================================
// Two-Hand Safety Controller
// Modified Version
// Emergency using Pull-Up Button
// ======================================

// -------- Pin Definitions --------
const int touchPin = 13;
const int workButton = 8;      // Pull-down button
const int emergencyButton = 9; // Pull-up button

const int trigPin = 6;
const int echoPin = 7;

const int redLED = 12;
const int greenLED = 11;
const int buzzer = 10;

// -------- Variables --------
bool emergency = false;
bool lastEmergencyState = HIGH;

void setup()
{
  Serial.begin(9600);

  pinMode(touchPin, INPUT);
  pinMode(workButton, INPUT);

  // External Pull-up Resistor Used
  pinMode(emergencyButton, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(buzzer, LOW);
}

void loop()
{
  // ==============================
  // Emergency Toggle Button
  // ==============================

  bool emergencyState = digitalRead(emergencyButton);

  if (lastEmergencyState == HIGH && emergencyState == LOW)
  {
    emergency = !emergency;
    delay(200);      // Debounce
  }

  lastEmergencyState = emergencyState;

  // ==============================
  // Emergency Mode
  // ==============================

  if (emergency)
  {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzer, HIGH);

    Serial.println("EMERGENCY MODE");

    return;
  }

  // ==============================
  // Read Sensors
  // ==============================

  bool touch = digitalRead(touchPin);
  bool button = digitalRead(workButton);

  // Ultrasonic Measurement

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  float distance = duration * 0.0343 / 2.0;

  // ==============================
  // Serial Monitor
  // ==============================

  Serial.print("Touch: ");
  Serial.print(touch);

  Serial.print("  Button: ");
  Serial.print(button);

  Serial.print("  Distance: ");
  Serial.print(distance);

  Serial.print(" cm  Emergency: ");
  Serial.println(emergency);

  // ==============================
  // Safety Logic
  // ==============================

  if (touch == HIGH && button == HIGH && distance <= 30)
  {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
  }
  else
  {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
  }

  delay(50);
}
