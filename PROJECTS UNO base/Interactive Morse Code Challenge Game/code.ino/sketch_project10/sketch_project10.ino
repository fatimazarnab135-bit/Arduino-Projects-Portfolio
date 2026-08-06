/*
  Project #10: Interactive Morse Code Game (Fixed Detection & Verification)
  -----------------------------------------------------------------------
  - Push Button (Pin 5): Start / Next Challenge
  - Touch Sensor (Pin 2): Space (' ')
  - Ultrasonic Sensor (Pins 3/4): Input (< 10cm)
    * Fast (< 350ms) = Dot (.)
    * Hold (>= 350ms) = Dash (-)
  - Pause > 2.5 seconds = Automatic Answer Verification!
*/

const int touchPin = 2;
const int trigPin = 3;
const int echoPin = 4;
const int buttonPin = 5;
const int redLedPin = 8;
const int greenLedPin = 9;
const int buzzerPin = 10;

String targets[] = {"SOS", "HI", "OK", "CAT", "GO"};
int currentTargetIndex = -1;
String currentTargetWord = "";
String currentTargetMorse = "";
String enteredMorse = "";

unsigned long keyPressStartTime = 0;
unsigned long lastInputTime = 0;
bool isKeyActive = false;
bool isWaitingForNextButton = true;

int lastTouchState = LOW;
int lastButtonState = HIGH;

String getMorseForWord(String word) {
  if (word == "SOS") return "... --- ...";
  if (word == "HI")  return ".... ..";
  if (word == "OK")  return "--- -.-";
  if (word == "CAT") return "-.-. .- -";
  if (word == "GO")  return "--. ---";
  return "";
}

void printInstructions() {
  Serial.println("\n==================================================");
  Serial.println("           INTERACTIVE MORSE GAME                 ");
  Serial.println("==================================================");
  Serial.println("INSTRUCTIONS:");
  Serial.println("1. Hold hand < 10cm for < 0.35 sec  --> Dot (.)");
  Serial.println("2. Hold hand < 10cm for >= 0.35 sec --> Dash (-)");
  Serial.println("3. Tap Touch Sensor                 --> Space (' ')");
  Serial.println("4. Stop moving for 2.5 sec          --> Auto Verify!");
  Serial.println("5. Press Push Button to get a NEW target word!");
  Serial.println("==================================================\n");
}

void setup() {
  pinMode(touchPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  printInstructions();
  Serial.println(">> Press PUSH BUTTON to start your first word challenge! <<");
}

void startNewChallenge() {
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(buzzerPin, LOW);
  
  enteredMorse = "";
  currentTargetIndex = (currentTargetIndex + 1) % 5;
  currentTargetWord = targets[currentTargetIndex];
  currentTargetMorse = getMorseForWord(currentTargetWord);
  isWaitingForNextButton = false;
  lastInputTime = 0;

  Serial.println("\n--------------------------------------------------");
  Serial.print("TARGET WORD  : ");
  Serial.println(currentTargetWord);
  Serial.print("TARGET MORSE : ");
  Serial.println(currentTargetMorse);
  Serial.println("--------------------------------------------------");
  Serial.print("Your Input   : ");
}

void verifyAnswer() {
  String trimmedEntered = enteredMorse;
  trimmedEntered.trim();
  String trimmedTarget = currentTargetMorse;
  trimmedTarget.trim();

  Serial.println("\n\nChecking input...");

  if (trimmedEntered == trimmedTarget) {
    // CORRECT ANSWER: Green LED + Buzzer ON/OFF every 0.5s
    Serial.println("🎉 CORRECT ANSWER! PERFECT MORSE CODE MATCH! 🎉");
    Serial.println(">> Press PUSH BUTTON for the next word challenge! <<");
    
    for (int i = 0; i < 4; i++) {
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(500);
      digitalWrite(greenLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
      delay(500);
    }
  } else {
    // WRONG ANSWER: Continuous Red LED & Buzzer + Explanation
    Serial.println("❌ INCORRECT ANSWER!");
    Serial.println("--------------- ERROR ANALYSIS ---------------");
    Serial.print("Expected Target : ");
    Serial.println(trimmedTarget);
    Serial.print("What You Entered: ");
    Serial.println(trimmedEntered);
    
    if (trimmedEntered.length() != trimmedTarget.length()) {
      Serial.println("Reason          : Length Mismatch (Missing or extra dots/dashes/spaces)");
    } else {
      Serial.println("Reason          : Pattern Mismatch (Incorrect dot/dash sequence)");
    }
    Serial.println("----------------------------------------------");
    Serial.println(">> Press PUSH BUTTON to try again with a new word! <<");

    digitalWrite(redLedPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(2500);
    digitalWrite(redLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  isWaitingForNextButton = true;
}

void loop() {
  // 1. Push Button Check
  int currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    digitalWrite(buzzerPin, HIGH);
    delay(50);
    digitalWrite(buzzerPin, LOW);
    
    startNewChallenge();
    delay(200);
  }
  lastButtonState = currentButtonState;

  if (isWaitingForNextButton) {
    return;
  }

  // 2. Touch Sensor Check (Space)
  int currentTouchState = digitalRead(touchPin);
  if (currentTouchState == HIGH && lastTouchState == LOW) {
    enteredMorse += " ";
    Serial.print(" ");
    lastInputTime = millis();
    
    digitalWrite(buzzerPin, HIGH);
    delay(40);
    digitalWrite(buzzerPin, LOW);
  }
  lastTouchState = currentTouchState;

  // 3. Ultrasonic Sensor Reading
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distanceCm = duration * 0.034 / 2;
  bool isHandDetected = (distanceCm > 0 && distanceCm < 10);

  // 4. Handle Morse Keying
  if (isHandDetected) {
    if (!isKeyActive) {
      isKeyActive = true;
      keyPressStartTime = millis();
      
      digitalWrite(greenLedPin, LOW);
      digitalWrite(redLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
    }
  } else {
    if (isKeyActive) {
      unsigned long holdDuration = millis() - keyPressStartTime;
      
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
      
      isKeyActive = false;

      if (holdDuration < 350) {
        enteredMorse += ".";
        Serial.print(".");
      } else {
        enteredMorse += "-";
        Serial.print("-");
      }
      
      lastInputTime = millis(); // Reset timer for auto-verify
    }
  }

  // 5. Auto-Verify Timeout (2.5 Seconds of No Input)
  if (enteredMorse.length() > 0 && lastInputTime > 0 && (millis() - lastInputTime > 2500) && !isKeyActive) {
    verifyAnswer();
  }

  delay(30);
}
