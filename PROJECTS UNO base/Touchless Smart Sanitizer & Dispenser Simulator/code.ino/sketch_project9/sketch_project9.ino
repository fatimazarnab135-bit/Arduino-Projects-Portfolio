/*
  Project #9: Touchless Smart Sanitizer & Dispenser Simulator
  -----------------------------------------------------------
  - HC-SR04 Ultrasonic Sensor: Detects hand proximity (< 10 cm)
  - Touch Sensor: System Lock / Refill Toggle Button
  - Red LED: Dispense Process Active
  - Green LED: System Ready Indicator
  - Active Buzzer: Pump Impulse Feedback
*/

const int touchPin = 2;
const int trigPin = 3;
const int echoPin = 4;
const int redLedPin = 8;    // Dispensing LED
const int greenLedPin = 9;  // Ready State LED
const int buzzerPin = 10;   // Pump Sound

bool isSystemActive = true;
bool handPreviouslyDetected = false;
int lastTouchState = LOW;

void setup() {
  pinMode(touchPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  Serial.println("--- Smart Sanitizer Simulator Ready ---");
}

void loop() {
  // 1. Read Touch Sensor to Toggle System Lock / Unlock
  int currentTouchState = digitalRead(touchPin);
  if (currentTouchState == HIGH && lastTouchState == LOW) {
    isSystemActive = !isSystemActive; // Toggle state
    
    // Audio Feedback for Toggle
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(200);
  }
  lastTouchState = currentTouchState;

  // 2. Main Dispenser Operation
  if (isSystemActive) {
    digitalWrite(greenLedPin, HIGH); // Green ON = Ready for Hand

    // Measure Distance
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int distanceCm = duration * 0.034 / 2;

    // Check if Hand is closer than 10 cm
    if (distanceCm > 0 && distanceCm < 10) {
      if (!handPreviouslyDetected) {
        // DISPENSE TRIGGERED
        Serial.println("Hand Detected! Dispensing Liquid...");
        
        digitalWrite(greenLedPin, LOW);
        digitalWrite(redLedPin, HIGH);
        
        // Simulate Pump Activation (Buzzer Beep)
        digitalWrite(buzzerPin, HIGH);
        delay(250); 
        digitalWrite(buzzerPin, LOW);
        
        delay(500); // Dispense duration
        digitalWrite(redLedPin, LOW);
        
        handPreviouslyDetected = true; // Lock until hand moves away
      }
    } else {
      handPreviouslyDetected = false; // Reset lock when hand is removed
    }

  } else {
    // SYSTEM LOCKED / REFILL MODE
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  delay(50);
}
