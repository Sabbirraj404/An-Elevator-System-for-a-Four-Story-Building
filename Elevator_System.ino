#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo door;

/* Motor */
int enA = 9;
int in1 = 8;
int in2 = 7;

/* Buzzer */
#define BUZZER 10

/* Buttons */
int F1 = 2;
int F2 = 3;
int F3 = 4;
int F4 = 5;

/* Queue */
#define MAX_Q 10
int queue[MAX_Q];
int front = 0, rear = 0;

/* Elevator */
int currentFloor = 1;
int targetFloor = -1;

/* STATES */
enum State {IDLE, WAITING, MOVING, DOOR, DOOR_CLOSING};
State state = IDLE;

/* Timers */
unsigned long moveTimer = 0;
unsigned long doorTimer = 0;
unsigned long buzzerTimer = 0;
unsigned long startDelayTimer = 0;

#define MOVE_DELAY 800
#define DOOR_DELAY 2000
#define BUTTON_DELAY 150
#define START_DELAY 1500
#define DOOR_CLOSE_DELAY 300   // 🔥 instead of delay()

bool buzzerOn = false;

/* Button states */
bool lastState[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounce[4] = {0,0,0,0};

/* ================= SETUP ================= */
void setup() {

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(F1, INPUT_PULLUP);
  pinMode(F2, INPUT_PULLUP);
  pinMode(F3, INPUT_PULLUP);
  pinMode(F4, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);

  door.write(0);
  delay(200);
  door.attach(6);

  lcd.init();
  lcd.backlight();

  stopMotor();
  closeDoor();
}

/* ================= LOOP ================= */
void loop() {

  readButtons();
  updateLCD();
  handleBuzzer();

  switch(state) {

    case IDLE:

      closeDoor();

      if(front != rear) {
        startDelayTimer = millis();
        state = WAITING;
      }
      break;

    case WAITING:

      if(millis() - startDelayTimer >= START_DELAY) {

        targetFloor = getNext();

        if(targetFloor != -1 && targetFloor != currentFloor) {

          if(targetFloor > currentFloor) moveUp();
          else moveDown();

          moveTimer = millis();
          state = MOVING;
        } else {
          state = IDLE;
        }
      }
      break;

    case MOVING:

      if(millis() - moveTimer >= MOVE_DELAY) {

        if(targetFloor > currentFloor) currentFloor++;
        else currentFloor--;

        beep();

        if(currentFloor == targetFloor) {
          stopMotor();
          targetFloor = -1;
          openDoor();
          doorTimer = millis();
          state = DOOR;
        } else {
          moveTimer = millis();
        }
      }
      break;

    case DOOR:

      if(millis() - doorTimer >= DOOR_DELAY) {

        closeDoor();
        doorTimer = millis();   // reuse timer
        state = DOOR_CLOSING;
      }
      break;

    case DOOR_CLOSING:

      if(millis() - doorTimer >= DOOR_CLOSE_DELAY) {
        state = IDLE;
      }
      break;
  }
}

/* ================= BUTTON ================= */
void readButtons() {

  int buttons[4] = {F1, F2, F3, F4};

  for(int i = 0; i < 4; i++) {

    bool currentState = digitalRead(buttons[i]);

    if(lastState[i] == HIGH && currentState == LOW) {

      if(millis() - lastDebounce[i] > BUTTON_DELAY) {

        addToQueue(i + 1);
        lastDebounce[i] = millis();
      }
    }

    lastState[i] = currentState;
  }
}

/* ================= QUEUE ================= */
void addToQueue(int floor) {

  for(int i = front; i != rear; i = (i + 1) % MAX_Q) {
    if(queue[i] == floor) return;
  }

  int next = (rear + 1) % MAX_Q;

  if(next != front) {
    queue[rear] = floor;
    rear = next;
  }
}

int getNext() {

  if(front == rear) return -1;

  int val = queue[front];
  front = (front + 1) % MAX_Q;

  return val;
}

/* ================= MOTOR ================= */
void moveUp() {
  analogWrite(enA, 200);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void moveDown() {
  analogWrite(enA, 200);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void stopMotor() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

/* ================= SERVO ================= */
void openDoor() {
  door.write(90);
}

void closeDoor() {
  door.write(0);
}

/* ================= BUZZER ================= */
void beep() {
  tone(BUZZER, 1500);
  buzzerOn = true;
  buzzerTimer = millis();
}

void handleBuzzer() {
  if(buzzerOn && millis() - buzzerTimer > 50) {
    noTone(BUZZER);
    buzzerOn = false;
  }
}

/* ================= LCD ================= */
void updateLCD() {

  lcd.setCursor(0,0);
  lcd.print("Floor: ");
  lcd.print(currentFloor);
  lcd.print("   ");

  lcd.setCursor(0,1);

  if(state == WAITING)
    lcd.print("Waiting...   ");
  else if(state == MOVING && targetFloor > currentFloor)
    lcd.print("UP   ^       ");
  else if(state == MOVING && targetFloor < currentFloor)
    lcd.print("DOWN v       ");
  else if(state == DOOR || state == DOOR_CLOSING)
    lcd.print("Door Open  ");
  else
    lcd.print("Door Closed         ");
}