/**
 * Warehouse Crane Simulator with Task Validation
 * Target Board: Arduino Uno
 * Complete Consolidated Code
 */

#include <Arduino.h>

// ==========================================
// 1. PIN CONFIGURATIONS
// ==========================================
const int PIN_JOYSTICK_VRX = A0;   // Joystick X-axis
const int PIN_JOYSTICK_VRY = A1;   // Joystick Y-axis
const int PIN_JOYSTICK_SW  = 2;    // Joystick Push Button
const int PIN_TOUCH_SENSOR = 3;    // Touch Sensor (Capacitive)
const int PIN_LED_RED      = 6;    // Red LED
const int PIN_LED_GREEN    = 7;    // Green LED
const int PIN_BUZZER       = 8;    // Active Buzzer
const int PIN_LED_YELLOW   = 9;    // Yellow LED
const int PIN_PUSH_BUTTON  = 10;   // Main Task Controller Button

// ==========================================
// 2. CONSTANTS & SYSTEM ENUMS
// ==========================================
enum MoveType {
    MOVE_NONE,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_STOP,
    MOVE_PICK,
    MOVE_DROP
};

const char* const MOVE_STRINGS[] = {
    "NONE", "UP", "DOWN", "LEFT", "RIGHT", "STOP", "PICK", "DROP"
};

enum SystemState {
    STATE_IDLE,
    STATE_TASK_ACTIVE,
    STATE_EVALUATION_SUCCESS,
    STATE_EVALUATION_FAILURE
};

#define MAX_MOVES_PER_TASK 20
#define TOTAL_TASKS 3

struct CraneTask {
    const char* description;
    MoveType expectedMoves[MAX_MOVES_PER_TASK];
    int totalMovesCount;
};

// Predefined routes database
const CraneTask taskDatabase[TOTAL_TASKS] = {
    {
        "Task 1: Move UP, Move LEFT, Move DOWN, STOP Crane, Pick Parcel. Then Move UP, Move LEFT, Move DOWN, STOP Crane, Release Parcel.",
        {MOVE_UP, MOVE_LEFT, MOVE_DOWN, MOVE_STOP, MOVE_PICK, MOVE_UP, MOVE_LEFT, MOVE_DOWN, MOVE_STOP, MOVE_DROP},
        10
    },
    {
        "Task 2: Move RIGHT, Move UP, Move LEFT, Move DOWN, STOP Crane, Pick Parcel. Then Move UP, Move RIGHT, STOP Crane, Release Parcel.",
        {MOVE_RIGHT, MOVE_UP, MOVE_LEFT, MOVE_DOWN, MOVE_STOP, MOVE_PICK, MOVE_UP, MOVE_RIGHT, MOVE_STOP, MOVE_DROP},
        10
    },
    {
        "Task 3: Move DOWN, Move LEFT, STOP Crane, Pick Parcel. Then Move UP, Move RIGHT, Move DOWN, STOP Crane, Release Parcel.",
        {MOVE_DOWN, MOVE_LEFT, MOVE_STOP, MOVE_PICK, MOVE_UP, MOVE_RIGHT, MOVE_DOWN, MOVE_STOP, MOVE_DROP},
        9
    }
};

// ==========================================
// 3. GLOBAL STATE VARIABLES
// ==========================================
SystemState currentState = STATE_IDLE;
int currentTaskIndex = 0;

MoveType userMovesHistory[MAX_MOVES_PER_TASK];
int userMoveCount = 0;

bool isCraneRunning = false;       // Governed by Joystick Switch
bool isParcelPicked = false;       // Tracks touch state sequence

unsigned long lastMainButtonDebounce  = 0;
unsigned long lastJoystickSwitchDebounce = 0;
unsigned long lastTouchDebounce          = 0;
const unsigned long DEBOUNCE_DELAY       = 250; 

// Timing variables for asynchronous blinking
unsigned long lastBlinkerMillis = 0;
bool alertToggleState = false;
unsigned long lastRedBlinkMillis = 0;
bool redBlinkState = false;

// Joystick Calibration Defaults
const int JOYSTICK_THRESHOLD_LOW  = 350;
const int JOYSTICK_THRESHOLD_HIGH = 650;
MoveType lastRegisteredJoystickMove = MOVE_NONE;
unsigned long joystickHoldTimer     = 0;
const unsigned long JOYSTICK_HOLD_DELAY = 800; 

// ==========================================
// 4. FUNCTION DECLARATIONS
// ==========================================
void processMainButton();
void processJoystickSwitch();
void processTouchSensor();
void processCraneMovement();
void updateLEDOutputs();
void validateUserPerformance();
void printTaskDescription(int index);
void resetSystemVariables();

// ==========================================
// 5. SETUP & MAIN LOOP
// ==========================================
void setup() {
    Serial.begin(9600);
    while (!Serial) { ; }

    pinMode(PIN_JOYSTICK_SW, INPUT_PULLUP);
    pinMode(PIN_TOUCH_SENSOR, INPUT);
    pinMode(PIN_PUSH_BUTTON, INPUT_PULLUP);
    
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    resetSystemVariables();
    
    Serial.println(F("========================================"));
    Serial.println(F("      WAREHOUSE CRANE SIMULATOR         "));
    Serial.println(F("========================================"));
    Serial.println(F("Press Main Push Button to Start Task\n"));
}

void loop() {
    processMainButton();

    if (currentState == STATE_TASK_ACTIVE) {
        processJoystickSwitch();
        processTouchSensor();
        processCraneMovement();
    }

    updateLEDOutputs();
}

// ==========================================
// 6. CORE LOGIC & HARDWARE CONTEXT HANDLING
// ==========================================
void processMainButton() {
    int buttonState = digitalRead(PIN_PUSH_BUTTON);
    if (buttonState == LOW && (millis() - lastMainButtonDebounce > DEBOUNCE_DELAY)) {
        lastMainButtonDebounce = millis();

        if (currentState == STATE_IDLE) {
            resetSystemVariables();
            currentState = STATE_TASK_ACTIVE;
            
            Serial.print(F("Task ")); 
            Serial.print(currentTaskIndex + 1); 
            Serial.println(F(" Started"));
            printTaskDescription(currentTaskIndex);
        } 
        else if (currentState == STATE_TASK_ACTIVE) {
            Serial.print(F("Task ")); 
            Serial.print(currentTaskIndex + 1); 
            Serial.println(F(" Completed"));
            Serial.println(F("Checking Task..."));
            
            validateUserPerformance();
        } 
        else if (currentState == STATE_EVALUATION_SUCCESS || currentState == STATE_EVALUATION_FAILURE) {
            currentTaskIndex = (currentTaskIndex + 1) % TOTAL_TASKS;
            currentState = STATE_IDLE;
            resetSystemVariables();
            
            Serial.println(F("\nPress Main Push Button to Start Next Task"));
        }
    }
}

void processJoystickSwitch() {
    int swState = digitalRead(PIN_JOYSTICK_SW);
    if (swState == LOW && (millis() - lastJoystickSwitchDebounce > DEBOUNCE_DELAY)) {
        lastJoystickSwitchDebounce = millis();

        isCraneRunning = !isCraneRunning; 

        if (isCraneRunning) {
            Serial.println(F("Crane Started"));
        } else {
            Serial.println(F("Crane Stopped"));
            
            if (userMoveCount < MAX_MOVES_PER_TASK) {
                userMovesHistory[userMoveCount] = MOVE_STOP;
                userMoveCount++;
                Serial.print(F("Move ")); 
                Serial.print(userMoveCount); 
                Serial.println(F(": STOP"));
            }
        }
    }
}

void processTouchSensor() {
    // Touch only works when crane is completely STOPPED
    if (!isCraneRunning && currentState == STATE_TASK_ACTIVE) {
        int touchState = digitalRead(PIN_TOUCH_SENSOR);
        if (touchState == HIGH && (millis() - lastTouchDebounce > DEBOUNCE_DELAY)) {
            lastTouchDebounce = millis();

            if (userMoveCount < MAX_MOVES_PER_TASK) {
                if (!isParcelPicked) {
                    isParcelPicked = true;
                    userMovesHistory[userMoveCount] = MOVE_PICK;
                    userMoveCount++;
                    Serial.print(F("Move ")); 
                    Serial.print(userMoveCount); 
                    Serial.println(F(": PICK"));
                    Serial.println(F("Parcel Picked Successfully"));
                } else {
                    isParcelPicked = false;
                    userMovesHistory[userMoveCount] = MOVE_DROP;
                    userMoveCount++;
                    Serial.print(F("Move ")); 
                    Serial.print(userMoveCount); 
                    Serial.println(F(": DROP"));
                    Serial.println(F("Parcel Released Successfully"));
                }
            }
        }
    }
}

void processCraneMovement() {
    if (!isCraneRunning) {
        lastRegisteredJoystickMove = MOVE_NONE;
        return;
    }

    int xVal = analogRead(PIN_JOYSTICK_VRX);
    int yVal = analogRead(PIN_JOYSTICK_VRY);
    MoveType currentReading = MOVE_NONE;

    // Direct mapping configuration: UP reading processes as UP
    if (yVal < JOYSTICK_THRESHOLD_LOW) {
        currentReading = MOVE_UP;
    } else if (yVal > JOYSTICK_THRESHOLD_HIGH) {
        currentReading = MOVE_DOWN;
    } else if (xVal < JOYSTICK_THRESHOLD_LOW) {
        currentReading = MOVE_LEFT;
    } else if (xVal > JOYSTICK_THRESHOLD_HIGH) {
        currentReading = MOVE_RIGHT;
    }

    if (currentReading != MOVE_NONE) {
        if (currentReading != lastRegisteredJoystickMove || (millis() - joystickHoldTimer > JOYSTICK_HOLD_DELAY)) {
            lastRegisteredJoystickMove = currentReading;
            joystickHoldTimer = millis();

            if (userMoveCount < MAX_MOVES_PER_TASK) {
                userMovesHistory[userMoveCount] = currentReading;
                userMoveCount++;
                
                Serial.print(F("Move "));
                Serial.print(userMoveCount);
                Serial.print(F(": "));
                Serial.println(MOVE_STRINGS[currentReading]);
            }
        }
    } else {
        lastRegisteredJoystickMove = MOVE_NONE;
    }
}

// ==========================================
// 7. DYNAMIC LED & ALERT MANAGER
// ==========================================
void updateLEDOutputs() {
    unsigned long currentMillis = millis();

    switch (currentState) {
        case STATE_IDLE:
            digitalWrite(PIN_LED_RED, HIGH);
            digitalWrite(PIN_LED_GREEN, LOW);
            digitalWrite(PIN_LED_YELLOW, LOW);
            digitalWrite(PIN_BUZZER, LOW);
            break;

        case STATE_TASK_ACTIVE:
            digitalWrite(PIN_LED_YELLOW, LOW);
            digitalWrite(PIN_BUZZER, LOW);

            if (isCraneRunning) {
                // Crane Moving profile: Red Blinking, Green OFF
                digitalWrite(PIN_LED_GREEN, LOW);
                if (currentMillis - lastRedBlinkMillis >= 250) { // Fast pacing for active motion
                    lastRedBlinkMillis = currentMillis;
                    redBlinkState = !redBlinkState;
                    digitalWrite(PIN_LED_RED, redBlinkState);
                }
            } else {
                // Crane Stopped / Picking / Dropping profile: Red OFF, Green ON
                digitalWrite(PIN_LED_RED, LOW);
                digitalWrite(PIN_LED_GREEN, HIGH);
            }
            break;

        case STATE_EVALUATION_SUCCESS:
            // Success profile: Red OFF, Green ON, Yellow + Buzzer Blinking at 0.5s intervals
            digitalWrite(PIN_LED_RED, LOW);
            digitalWrite(PIN_LED_GREEN, HIGH);
            
            if (currentMillis - lastBlinkerMillis >= 500) {
                lastBlinkerMillis = currentMillis;
                alertToggleState = !alertToggleState;
                
                digitalWrite(PIN_LED_YELLOW, alertToggleState);
                digitalWrite(PIN_BUZZER, alertToggleState);
            }
            break;

        case STATE_EVALUATION_FAILURE:
            // Failure profile: Red OFF, Green ON, Yellow/Buzzer completely dead
            digitalWrite(PIN_LED_RED, LOW);
            digitalWrite(PIN_LED_GREEN, HIGH);
            digitalWrite(PIN_LED_YELLOW, LOW);
            digitalWrite(PIN_BUZZER, LOW);
            break;
    }
}

// ==========================================
// 8. PERFORMANCE VALIDATION SUBROUTINE
// ==========================================
void validateUserPerformance() {
    const CraneTask target = taskDatabase[currentTaskIndex];
    bool evaluationPassed = true;
    int failedMoveIndex = -1;

    for (int i = 0; i < target.totalMovesCount; i++) {
        if (i >= userMoveCount || userMovesHistory[i] != target.expectedMoves[i]) {
            evaluationPassed = false;
            failedMoveIndex = i;
            break;
        }
    }

    if (evaluationPassed && userMoveCount > target.totalMovesCount) {
        evaluationPassed = false;
        failedMoveIndex = target.totalMovesCount;
    }

    if (evaluationPassed) {
        currentState = STATE_EVALUATION_SUCCESS;
        Serial.println(F("Description = task successfully completed"));
        lastBlinkerMillis = millis();
        alertToggleState = true;
    } else {
        currentState = STATE_EVALUATION_FAILURE;
        Serial.println(F("Task Failed"));
        
        if (failedMoveIndex < userMoveCount && failedMoveIndex >= 0) {
            Serial.print(F("Move "));
            Serial.print(failedMoveIndex + 1);
            Serial.println(F(" is Incorrect"));
            
            Serial.println(F("Expected :"));
            if (failedMoveIndex < target.totalMovesCount) {
                Serial.println(MOVE_STRINGS[target.expectedMoves[failedMoveIndex]]);
            } else {
                Serial.println(F("END OF TASK"));
            }
            
            Serial.println(F("You Performed :"));
            Serial.println(MOVE_STRINGS[userMovesHistory[failedMoveIndex]]);
        } else {
            Serial.print(F("Move "));
            Serial.print(failedMoveIndex + 1);
            Serial.println(F(" is Incorrect"));
            Serial.println(F("Expected :"));
            Serial.println(MOVE_STRINGS[target.expectedMoves[failedMoveIndex]]);
            Serial.println(F("You Performed :"));
            Serial.println(F("NONE (Premature Task Termination)"));
        }
    }
}

void printTaskDescription(int index) {
    Serial.println(F("----------------------------------------"));
    Serial.println(taskDatabase[index].description);
    Serial.println(F("----------------------------------------"));
}

void resetSystemVariables() {
    userMoveCount = 0;
    isCraneRunning = false;
    isParcelPicked = false;
    lastRegisteredJoystickMove = MOVE_NONE;
    
    for (int i = 0; i < MAX_MOVES_PER_TASK; i++) {
        userMovesHistory[i] = MOVE_NONE;
    }
}