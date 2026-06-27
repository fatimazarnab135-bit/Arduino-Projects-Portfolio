// ================= PIN SETUP =================
const int buttons[9] = {2,3,4,5,6,7,8,9,10};

const int greenLED = 11;
const int redLED   = 12;
const int buzzer   = 13;
const int yellowLED = A0;

// ================= SETTINGS =================
#define LONG_TIME 1000   // >= 1 sec = LONG
#define CODE_LEN 6
#define MAX_ROUNDS 3
#define MAX_ATTEMPTS 3

// ================= PASSWORDS (S=0, L=1) =================
int codes[MAX_ROUNDS][CODE_LEN] = {
  {1,0,1,0,0,1}, // Round 1
  {0,1,0,1,0,1}, // Round 2
  {1,0,1,0,1,0}  // Round 3
};

// ================= STATE =================
int roundNum = 0;
int attempt = 0;
int inputIndex = 0;

int userInput[CODE_LEN];

// Button states
bool buttonActive[9] = {false};
unsigned long pressStart[9];
unsigned long lastReleaseTime[9] = {0};

// ================= SETUP =================
void setup() {
  Serial.begin(9600);

  for(int i=0;i<9;i++){
    pinMode(buttons[i], INPUT_PULLUP);
  }

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  showRound();
}

// ================= LOOP =================
void loop() {

  if(attempt >= MAX_ATTEMPTS){
    systemLock();
    return;
  }

  for(int i=0;i<9;i++){

    int state = digitalRead(buttons[i]);

    // ===== PRESS START =====
    if(state == LOW && !buttonActive[i]){
      buttonActive[i] = true;
      pressStart[i] = millis();
    }

    // ===== RELEASE =====
    if(state == HIGH && buttonActive[i]){
      buttonActive[i] = false;

      unsigned long pressTime = millis() - pressStart[i];

      // ===== DEBOUNCE FILTER =====
      if(pressTime < 50) continue;

      // ===== ANTI DOUBLE TRIGGER =====
      if(millis() - lastReleaseTime[i] < 150) continue;
      lastReleaseTime[i] = millis();

      int type = (pressTime >= LONG_TIME) ? 1 : 0;

      handleInput(i, type);
    }
  }
}

// ================= HANDLE INPUT =================
void handleInput(int button, int type){

  if(inputIndex >= CODE_LEN) return;

  userInput[inputIndex] = type;

  Serial.print("Button ");
  Serial.print(button + 1);
  Serial.print(" => ");
  Serial.println(type == 1 ? "LONG" : "SHORT");

  inputIndex++;

  Serial.print("Step ");
  Serial.print(inputIndex);
  Serial.println("/6");

  if(inputIndex >= CODE_LEN){
    checkCode();
  }
}

// ================= CHECK CODE =================
void checkCode(){

  bool correct = true;

  for(int i=0;i<CODE_LEN;i++){
    if(userInput[i] != codes[roundNum][i]){
      correct = false;
      break;
    }
  }

  inputIndex = 0;

  if(correct){
    successRound();
  } else {
    failAttempt();
  }
}

// ================= SUCCESS =================
void successRound(){

  Serial.println("ROUND CLEARED!");

  digitalWrite(greenLED, HIGH);
  tone(buzzer, 1000, 200);
  delay(2000);
  digitalWrite(greenLED, LOW);

  roundNum++;

  if(roundNum >= MAX_ROUNDS){
    accessGranted();
  } else {
    showRound();
  }
}

// ================= FAIL =================
void failAttempt(){

  attempt++;

  Serial.println("ACCESS DENIED");

  digitalWrite(redLED, HIGH);
  tone(buzzer, 500, 300);
  delay(1000);
  digitalWrite(redLED, LOW);

  Serial.print("Attempts: ");
  Serial.println(attempt);

  if(attempt >= MAX_ATTEMPTS){
    systemLock();
  }
}

// ================= ACCESS GRANTED =================
void accessGranted(){

  Serial.println("ACCESS GRANTED");

  digitalWrite(greenLED, HIGH);
  tone(buzzer, 1200, 800);

  delay(3000);

  digitalWrite(greenLED, LOW);

  resetSystem();
}

// ================= SYSTEM LOCK =================
void systemLock(){

  Serial.println("SYSTEM LOCKED");

  digitalWrite(yellowLED, HIGH);

  unsigned long start = millis();

  while(millis() - start < 20000){
    tone(buzzer, 800);
  }

  noTone(buzzer);
  digitalWrite(yellowLED, LOW);

  resetSystem();
}

// ================= ROUND DISPLAY =================
void showRound(){

  Serial.print("ROUND ");
  Serial.println(roundNum + 1);
}

// ================= RESET =================
void resetSystem(){

  roundNum = 0;
  attempt = 0;
  inputIndex = 0;

  showRound();
}