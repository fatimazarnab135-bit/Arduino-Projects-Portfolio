int sensorPin = A0;
int redLED = 8;
int greenLED = 7;
int buzzer = 9;

int threshold = 700;

void setup()
{
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);
}

void loop()
{
  int moisture = analogRead(sensorPin);

  Serial.println(moisture);

  if (moisture > threshold)
  {
    // Dry soil
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzer, HIGH);
  }
  else
  {
    // Wet soil
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(buzzer, LOW);
  }

  delay(500);
}
