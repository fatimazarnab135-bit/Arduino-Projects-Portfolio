int xPin = A0;
int yPin = A1;

int touchPin = 3;

int red = 8;
int yellow = 9;
int green = 10;

int buzzer = 11;

int score = 0;

void setup() {
  Serial.begin(9600);

  pinMode(touchPin, INPUT);

  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);

  randomSeed(analogRead(A5));

  Serial.println("GAME STARTED");
}

void loop() {

  // WAIT FOR TOUCH TO START ROUND
  if (digitalRead(touchPin) == HIGH) {
    delay(300);

    int direction = random(1, 5); 
    // 1=UP, 2=DOWN, 3=LEFT, 4=RIGHT

    Serial.print("TARGET: ");
    Serial.println(direction);

    // show direction with LED
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);

    if (direction == 1) digitalWrite(red, HIGH);
    if (direction == 2) digitalWrite(red, HIGH);
    if (direction == 3) digitalWrite(yellow, HIGH);
    if (direction == 4) digitalWrite(green, HIGH);

    delay(1000);

    int x = analogRead(xPin);
    int y = analogRead(yPin);

    String move = "";

    // YOUR JOYSTICK MAPPING
    if (x < 200) move = "UP";
    else if (x > 800) move = "DOWN";
    else if (y > 800) move = "LEFT";
    else if (y < 200) move = "RIGHT";

    Serial.print("MOVE: ");
    Serial.println(move);

    bool correct = false;

    if (direction == 1 && move == "UP") correct = true;
    if (direction == 2 && move == "DOWN") correct = true;
    if (direction == 3 && move == "LEFT") correct = true;
    if (direction == 4 && move == "RIGHT") correct = true;

    if (correct) {
      score++;
      Serial.print("CORRECT! SCORE: ");
      Serial.println(score);

      digitalWrite(green, HIGH);
      tone(buzzer, 1000, 200);
      delay(500);
      digitalWrite(green, LOW);
    }
    else {
      Serial.print("WRONG! SCORE: ");
      Serial.println(score);

      digitalWrite(red, HIGH);
      tone(buzzer, 200, 500);
      delay(500);
      digitalWrite(red, LOW);
    }

    delay(1000);
  }
}
