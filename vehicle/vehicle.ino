#include <NewPing.h>

//motor 1 (MOTOR A, LEFT WHEEL)
#define ENA 7
#define IN1 6
#define IN2 5

//motor 2 (MOTOR B, RIGHT WHEEL)
#define ENB 2
#define IN3 4
#define IN4 3 

//motor speeds
#define MIN_SPEED 130
#define MAX_SPEED 255

//front sensor pins
#define TRIGF 8
#define ECHOF 9

//left sensor pins
#define TRIGL 10 
#define ECHOL 11

//right sensor pins
#define TRIGR 12
#define ECHOR 13

//sensor defines
#define OBJ_DIST 18   //distance obj is detected (inches)
#define MAX_DIST 500  //max dist sensors can see (centimeters)

//sensors
NewPing F_sensor(TRIGF, ECHOF, MAX_DIST);
NewPing L_sensor(TRIGL, ECHOL, MAX_DIST);
NewPing R_sensor(TRIGR, ECHOR, MAX_DIST);

//States
int State = 0;
enum {
  DRIVE_FORWARD = 0,
  DRIVE_BACKWARD = 1,
  TURN_LEFT = 2,
  TURN_RIGHT = 3
};

//sensor booleans
bool F_detected = false;
bool L_detected = false;
bool R_detected = false;

//drives vehicle forward
void driveForward() {
  //motor A (left wheel forward)
  analogWrite(ENA, MIN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  //motor B (right wheel forward)
  analogWrite(ENB, MIN_SPEED);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

//drives vehicle backward
void driveBackward() {
  //motor A (left wheel reverse)
  analogWrite(ENA, MIN_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  //motor B (right wheel reverse)
  analogWrite(ENB, MIN_SPEED);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

//stop vehicle
void stopCar() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

//turns vehicle left
void turnLeft() {
  //motor A (left wheel reverse)
  analogWrite(ENA, MIN_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  //motor B (right wheel forward)
  analogWrite(ENB, MIN_SPEED);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

//turns vehicle right
void turnRight() {
  //motor A (left wheel forward)
  analogWrite(ENA, MIN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  //motor B (right wheel reverse)
  analogWrite(ENB, MIN_SPEED);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

//checks if object detected
bool checkObject() {
  F_detected = false;
  L_detected = false;
  R_detected = false;
  if (L_sensor.ping_in() <= OBJ_DIST) {
    L_detected = true;
    return true;
   }
   if (R_sensor.ping_in() <= OBJ_DIST) {
    R_detected = true;
    return true;
   }
   if (F_sensor.ping_in() <= OBJ_DIST) {
    F_detected = true;
    return true;
  }
  return false;
}

//State machine function
void runCar() {
  switch (State) {
    case DRIVE_FORWARD:
      driveForward();
      if (checkObject()) {
        stopCar();
        State = DRIVE_BACKWARD;
      }
      break;
    case DRIVE_BACKWARD:
      driveBackward();
      //stop driving backwards after this distance
      if (F_sensor.ping_in() >= 1.5 * OBJ_DIST) {
        stopCar();
       //decide whether to turn left or right
       if (!L_detected&&R_detected) {
         State = TURN_LEFT;
       }
       else if ( (!F_detected&&L_detected) || (F_detected&&!L_detected&&!R_detected) || (F_detected&&L_detected) ) {
         State = TURN_RIGHT;
       }
      }
      break;
    case TURN_LEFT:
      turnLeft();
      if (!checkObject()) {
        State = DRIVE_FORWARD;
      }
      break;
    case TURN_RIGHT:
      turnRight();
      if (!checkObject()) {
        State = DRIVE_FORWARD;
      }
      break;
  }
}

void setup() {
  Serial.begin(9600);

  //motor pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  //sensor pins
  pinMode(ECHOF, INPUT);
  pinMode(ECHOR, INPUT);
  pinMode(ECHOL, INPUT);
  pinMode(TRIGF, OUTPUT);
  pinMode(TRIGR, OUTPUT);
  pinMode(TRIGL, OUTPUT);

  //initialize motors
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  runCar();
}
