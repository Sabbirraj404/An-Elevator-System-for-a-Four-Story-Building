#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo door;

// Motor 
int enA = 9;
int in1 = 8;
int in2 = 7;

// Buzzer
#define BUZZER 10

// Floor buttons
int btn[] = {2, 3, 4, 5};

// Queue
#define Q_SIZE 10
int q[Q_SIZE];
int f = 0, r = 0;

// Elevator info
int floorNow = 1;
int target = -1;

// States
enum State {IDLE, WAIT, MOVE, DOOR_OPEN, DOOR_CLOSE};
State st = IDLE;

// Timing
unsigned long tMove = 0;
unsigned long tDoor = 0;
unsigned long tStart = 0;
unsigned long tBuzz = 0;

#define MOVE_T 800
#define DOOR_T 2000
#define START_T 1500
#define BTN_T 150
#define CLOSE_T 300

bool buzzFlag = false;

// Button memory
bool lastBtn[4] = {1,1,1,1};
unsigned long lastPress[4] = {0,0,0,0};


void setup() {

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  for(int i=0;i<4;i++) {
    pinMode(btn[i], INPUT_PULLUP);
  }

  pinMode(BUZZER, OUTPUT);

  door.attach(6);
  door.write(0);

  lcd.init();
  lcd.backlight();

  stopMotor();
  closeDoor();
}


void loop() {

  checkButtons();
  showLCD();
  buzzerTask();

  switch(st) {

    case IDLE:

      closeDoor();

      if(f != r) {
        tStart = millis();
        st = WAIT;
      }
      break;

    case WAIT:

      if(millis() - tStart >= START_T) {

        target = pop();

        if(target != -1 && target != floorNow) {

          if(target > floorNow) up();
          else down();

          tMove = millis();
          st = MOVE;
        } else {
          st = IDLE;
        }
      }
      break;

    case MOVE:

      if(millis() - tMove >= MOVE_T) {

        if(target > floorNow) floorNow++;
        else floorNow--;

        beep();

        if(floorNow == target) {

          stopMotor();
          target = -1;

          openDoor();
          tDoor = millis();
          st = DOOR_OPEN;

        } else {
          tMove = millis();
        }
      }
      break;

    case DOOR_OPEN:

      if(millis() - tDoor >= DOOR_T) {
        closeDoor();
        tDoor = millis();
        st = DOOR_CLOSE;
      }
      break;

    case DOOR_CLOSE:

      if(millis() - tDoor >= CLOSE_T) {
        st = IDLE;
      }
      break;
  }
}


void checkButtons() {

  for(int i=0;i<4;i++) {

    bool cur = digitalRead(btn[i]);

    if(lastBtn[i] && !cur) {

      if(millis() - lastPress[i] > BTN_T) {
        push(i+1);
        lastPress[i] = millis();
      }
    }

    lastBtn[i] = cur;
  }
}


void push(int val) {

  for(int i=f; i!=r; i=(i+1)%Q_SIZE) {
    if(q[i] == val) return;
  }

  int next = (r+1)%Q_SIZE;

  if(next != f) {
    q[r] = val;
    r = next;
  }
}

int pop() {

  if(f == r) return -1;

  int v = q[f];
  f = (f+1)%Q_SIZE;
  return v;
}


void up() {
  analogWrite(enA, 200);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void down() {
  analogWrite(enA, 200);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void stopMotor() {
  analogWrite(enA, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}


void openDoor() {
  door.write(90);
}

void closeDoor() {
  door.write(0);
}


void beep() {
  tone(BUZZER, 1200);  
  buzzFlag = true;
  tBuzz = millis();
}

void buzzerTask() {
  if(buzzFlag && millis() - tBuzz > 50) {
    noTone(BUZZER);
    buzzFlag = false;
  }
}

// -------------------- LCD --------------------
void showLCD() {

  lcd.setCursor(0,0);
  lcd.print("Floor: ");
  lcd.print(floorNow);
  lcd.print("   ");

  lcd.setCursor(0,1);

  if(st == WAIT)
    lcd.print("Waiting...   ");
  else if(st == MOVE && target > floorNow)
    lcd.print("Going Up ^   ");
  else if(st == MOVE && target < floorNow)
    lcd.print("Going Down v ");
  else if(st == DOOR_OPEN)
    lcd.print("Door Open    ");
  else if(st == DOOR_CLOSE)
    lcd.print("Closing...   ");
  else
    lcd.print("Door Closed        ");
}