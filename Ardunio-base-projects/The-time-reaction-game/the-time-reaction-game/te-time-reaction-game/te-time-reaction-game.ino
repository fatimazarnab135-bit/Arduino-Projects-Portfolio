// Time Reaction Game
// LED -> Pin 8
// Buzzer -> Pin 9
// Button -> Pin 2

const int ledPin = 8;
const int buzzerPin = 9;
const int buttonPin = 2;

unsigned long bestReaction = 999999;
int bestRound = 0;
int roundNumber = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  Serial.begin(9600);

  randomSeed(analogRead(A0));

  Serial.println("====================================");
  Serial.println("      TIME REACTION GAME");
  Serial.println("====================================");
  Serial.println("Press 's' to start a game.");
  Serial.println("Press 'q' to quit.");
}

void loop() {

  if (Serial.available()) {

    char command = Serial.read();

    if (command == 's' || command == 'S') {

      roundNumber++;

      Serial.println();
      Serial.print("Round #");
      Serial.println(roundNumber);

      Serial.println("Get Ready...");

      // Random wait before LED
      delay(random(1000, 4000));

      digitalWrite(ledPin, HIGH);
      Serial.println("LED ON - Ready!");

      // Random wait before buzzer
      delay(random(1000, 3000));

      tone(buzzerPin, 1000);
      Serial.println("BUZZER ON - GO!");

      unsigned long startTime = millis();

      while (digitalRead(buttonPin) == LOW) {
        // Wait for player
      }

      unsigned long reactionTime = millis() - startTime;

      noTone(buzzerPin);
      digitalWrite(ledPin, LOW);

      Serial.print("Reaction Time: ");
      Serial.print(reactionTime);
      Serial.println(" ms");

      if (reactionTime < bestReaction) {
        bestReaction = reactionTime;
        bestRound = roundNumber;
      }

      Serial.println("------------------------------------");
      Serial.println("Press 's' for next game");
      Serial.println("Press 'q' to quit");
      Serial.println("------------------------------------");

      delay(2000);
    }

    else if (command == 'q' || command == 'Q') {

      Serial.println();
      Serial.println("====================================");

      if (bestRound > 0) {

        Serial.print("Wow CANDY HEART your least reaction time is: ");
        Serial.print(bestReaction);
        Serial.println(" ms");

        Serial.print("Achieved in Round #");
        Serial.println(bestRound);
      }
      else {
        Serial.println("No games played.");
      }

      Serial.println("====================================");

      while (true) {
        // Stop program
      }
    }
  }
}
