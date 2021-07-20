#include <Servo.h>
#include <NewPing.h>

//pins
#define TRIG 10
#define ECHO 11
#define TRIG2 5
#define ECHO2 6
#define SERVO_ATTACH 9
#define LED_DETECTED 8
#define LED_FOLLOWING 7
#define BTN A0

//sensor defines
#define MAX_DIST 15
#define ROTATION_SPEED 10

//defines for button press
#define high_vol 900

Servo servo;
NewPing sensor(TRIG, ECHO, MAX_DIST);
NewPing sensor2(TRIG2, ECHO2, MAX_DIST);

//states
enum {
  STATE_IDLE = 0,
  STATE_FOLLOW = 1
};

//rotation directions
enum {
  NONE = -1,
  COUNTER_CLOCKWISE = 0,
  CLOCKWISE = 1
};

//default state
int State = STATE_IDLE;

int checkRotation() {
  if (sensor.ping_cm() && !sensor2.ping_cm()) {
    return COUNTER_CLOCKWISE;
  }
  else if (!sensor.ping_cm() && sensor2.ping_cm()) {
    return CLOCKWISE;
  }
  return NONE;
}

void StateMachine() {
  static int angle = 0; //default angle in degrees
  static bool clockwise = false;
  
  switch (State) {
    case STATE_IDLE:
      //state transition
      if (sensor.ping_cm() || sensor2.ping_cm()) {
        State = STATE_FOLLOW;
      }

      //LEDs
      digitalWrite(LED_DETECTED, HIGH);
      digitalWrite(LED_FOLLOWING, LOW);

      //check if angle exceeds 180
      if (angle >= 180) {
        clockwise = true;
      } else if (angle <= 0) {
        clockwise = false;
      }
      
      //rotate motor
      delay(ROTATION_SPEED);
      if (!clockwise) {
        angle += 1;
      } else {
        angle -= 1;
      }
      delay(ROTATION_SPEED);
      servo.write(angle);
      
      break;
      
    case STATE_FOLLOW:
      //state transition
      if (analogRead(BTN) > high_vol) {
        State = STATE_IDLE;
      }
      
      //LEDs
      digitalWrite(LED_DETECTED, LOW);
      digitalWrite(LED_FOLLOWING, HIGH);

      //dont let motor reach 180 degrees
      if (servo.read() == 180) {
        servo.write(servo.read() - 1);
      }
      else if (servo.read() == 0) {
        servo.write(servo.read() + 1);
      }

      if (checkRotation() == COUNTER_CLOCKWISE) {
        servo.write(servo.read() + 1);
      } else if (checkRotation() == CLOCKWISE) {
        servo.write(servo.read() - 1);
      }
      
      break;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo.attach(SERVO_ATTACH);
  pinMode(ECHO, INPUT);
  pinMode(ECHO2, INPUT);
  pinMode(BTN, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(LED_DETECTED, OUTPUT);
  pinMode(LED_FOLLOWING, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  StateMachine();

}
