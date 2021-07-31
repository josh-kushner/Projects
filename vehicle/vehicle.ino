#include <NewPing.h>

//MOTOR 1 (MOTOR A, LEFT WHEEL)
#define ENA 7
#define IN1 6
#define IN2 5

//MOTOR 2 (MOTOR B, RIGHT WHEEL)
#define ENB 2
#define IN3 4
#define IN4 3 

//MOTOR SPEEDS
#define MIN_SPEED 130
#define MAX_SPEED 255

//FRONT SENSOR
#define TRIGF 8
#define ECHOF 9

//LEFT SENSOR
#define TRIGL 10 
#define ECHOL 11

//RIGHT SENSOR
#define TRIGR 12
#define ECHOR 13

//sensor variables
#define OBJ_DIST 18 //distance obj is detected (inches)
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

//sees if sensors detected anything
bool F_detected = false;
bool L_detected = false;
bool R_detected = false;

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

//checks if object detected in a specified sensor
bool checkObject(String sensor) {
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
  double L_sensor_dist;
  double R_sensor_dist;
  switch (State) {
    case DRIVE_FORWARD:
      driveForward();
      if (checkObject()) {
        //save L and R sensor distances, then state transition
        L_sensor_dist = L_sensor.ping_in();
        R_sensor_dist = R_sensor.ping_in();
        State = DRIVE_BACKWARD;
      }
      break;
    case DRIVE_BACKWARD:
      driveBackward();
      
      //stop driving backwards after this distance
      if (L_sensor.ping_in() >= L_sensor_dist + (OBJ_DIST/2)) {

        //see which direction has more space to turn the vehicle
        if (L_detected || (F_detected && L_detected)) {
          State = TURN_RIGHT;
        }
        else if (R_detected || (F_detected && R_detected)) {
          State = TURN_RIGHT;
        }
        else if (F_detected || (L_detected && R_detected) || (F_detected && L_detected && R_detected)) {
          if (R_sensor_dist <= L_sensor_dist) {
            State = TURN_RIGHT;
          }
          else if (L_sensor_dist < R_sensor_dist) {
            State = TURN_LEFT;
          }
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
      if (!checkObject)) {
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

  //initlialize motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  runCar();
}
