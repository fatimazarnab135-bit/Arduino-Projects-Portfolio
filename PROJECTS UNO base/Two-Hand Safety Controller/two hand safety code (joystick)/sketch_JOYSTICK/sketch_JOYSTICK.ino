// ======================================
// Two-Hand Safety Controller
// ======================================

// -------- Pin Definitions --------
const int touchPin = 2;
const int buttonPin = 3;
const int joySW = 4;

const int trigPin = 9;
const int echoPin = 10;

const int greenLED = 11;
const int buzzer = 12;
const int redLED = 13;

// -------- Variables --------
bool emergency = false;
bool lastJoyState = HIGH;

void setup()
{
  Serial.begin(9600);

  pinMode(touchPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(joySW, INPUT_PULLUP);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
}

void loop()
{
  // ===== Emergency Button =====
  bool joyState = digitalRead(joySW);

  if (lastJoyState == HIGH && joyState == LOW)
  {
    emergency = !emergency;
    delay(200); // debounce
  }

  lastJoyState = joyState;

  // ===== Emergency Mode =====
  if (emergency)
  {
    digitalWrite(greenLED, LOW);

    digitalWrite(redLED, HIGH);
    tone(buzzer, 1000);
    delay(500);

    digitalWrite(redLED, LOW);
    noTone(buzzer);
    delay(500);

    return;
  }

  // ===== Read Touch and Button =====
  bool touch = digitalRead(touchPin);
  bool button = digitalRead(buttonPin);

  // ===== Ultrasonic Distance =====
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  float distance = duration * 0.034 / 2.0;

  // ===== Serial Monitor =====
  Serial.print("Touch: ");
  Serial.print(touch);

  Serial.print("  Button: ");
  Serial.print(button);

  Serial.print("  Distance: ");
  Serial.print(distance);

  Serial.print(" cm  Emergency: ");
  Serial.println(emergency);

  // ===== Safety Logic =====
  if (touch == HIGH && button == HIGH && distance <= 30)
  {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    noTone(buzzer);
  }
  else
  {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    noTone(buzzer);
  }

  delay(50);
}
