#define TRIG 9
#define ECHO 10

#define GREEN 2
#define YELLOW 3
#define RED 4
#define BUZZER 8

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);
}

long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  long distance = duration * 0.034 / 2;

  return distance;
}

void loop() {
  long distance = getDistance();

  Serial.println(distance);

  // Reset all LEDs
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(BUZZER, LOW);

  if (distance > 30) {
    digitalWrite(GREEN, HIGH);
    delay(200);
  }

  else if (distance > 15 && distance <= 30) {
    digitalWrite(YELLOW, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(300);
    digitalWrite(BUZZER, LOW);
    delay(300);
  }

  else if (distance > 5 && distance <= 15) {
    digitalWrite(RED, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    delay(150);
  }

  else {
    digitalWrite(RED, HIGH);
    digitalWrite(BUZZER, HIGH);
  }
}

