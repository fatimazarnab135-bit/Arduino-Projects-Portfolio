/*
  Dual-Mode Anti-Intruder & Flood Alarm (Sensor Fusion System)
  -------------------------------------------------------------
  - Soil Moisture Sensor (A0): Detects Water Leakage / Flooding
  - Ultrasonic Sensor (Trig: D3, Echo: D4): Detects Intrusion (< 20cm)
  - Touch Sensor (D2): Mute / Safety Reset Button
  - Red LED (D8): Water Level Warning
  - Green LED (D9): Intruder Warning
  - Active Buzzer (D10): Multi-Tone Alarm System
*/

// Pin Configurations
const int touchPin = 2;
const int trigPin = 3;
const int echoPin = 4;
const int redLedPin = 8;
const int greenLedPin = 9;
const int buzzerPin = 10;
const int moisturePin = A0;

// System Thresholds
const int WATER_THRESHOLD = 500;      // Analog value below which water/flood is triggered
const int INTRUDER_DISTANCE_CM = 20;  // Intruder range threshold in centimeters

void setup() {
  // Initialize Sensor & Output Pins
  pinMode(touchPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Serial Monitor for System Diagnostics
  Serial.begin(9600);
  Serial.println("==============================================");
  Serial.println(" Dual-Mode Sensor Fusion Security System Ready");
  Serial.println("==============================================");
}

void loop() {
  // 1. Read Soil Moisture Sensor (Analog)
  int moistureVal = analogRead(moisturePin);
  bool isWaterDetected = (moistureVal < WATER_THRESHOLD);

  // 2. Read Ultrasonic Sensor Distance (Digital/Pulse)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distanceCm = duration * 0.034 / 2;
  
  // Filter out invalid 0cm readings
  bool isIntruderDetected = (distanceCm > 0 && distanceCm < INTRUDER_DISTANCE_CM);

  // 3. Read Touch Sensor State
  bool isTouchActive = (digitalRead(touchPin) == HIGH);

  // Real-Time System Telemetry on Serial Monitor
  Serial.print("Moisture: ");
  Serial.print(moistureVal);
  Serial.print(" | Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  // --- Sensor Fusion Priority Logic ---

  if (isTouchActive) {
    // SYSTEM OVERRIDE / RESET MODE
    // Touch sensor pressed: Silences all alarms instantly
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
    Serial.println("--> SYSTEM OVERRIDE: Muted by User.");
  }
  else if (isWaterDetected && isIntruderDetected) {
    // CRITICAL DUAL EMERGENCY (Water + Intruder)
    // Both LEDs turn ON + Fast Emergency Strobe Beep
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(60);
    digitalWrite(buzzerPin, LOW);
    delay(60);
  }
  else if (isWaterDetected) {
    // WATER LEAKAGE ALARM
    // Red LED ON + Rapid Warning Beep
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);
    delay(120);
    digitalWrite(buzzerPin, LOW);
    delay(120);
  }
  else if (isIntruderDetected) {
    // INTRUDER ALARM
    // Green LED ON + Slow Pulsing Beep
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
    delay(300);
  }
  else {
    // NORMAL MONITORING STATE
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  delay(40); // Small stability delay
}
