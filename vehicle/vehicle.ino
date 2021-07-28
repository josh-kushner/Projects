#include <L298NX2.h>

#define EN_A 1
#define IN1_A 2
#define IN2_A 3
#define EN_B 4
#define IN1_B 5
#define IN2_B 6

//declare instance of both motors
L298NX2 Motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);


//States
int State = 0;
enum {
  DRIVE_FORWARD = 0,
  DRIVE_BACKWARD = 1,
  TURN_LEFT = 2,
  TURN_RIGHT = 3,
  TURN_AROUND = 4
};

//State machine function
void driveCheck() {
  switch (State) {
    case DRIVE_FORWARD:
      break;
    case DRIVE_BACKWARD:
      break;
    case TURN_LEFT:
      break;
    case TURN_RIGHT:
      break;
    case TURN_AROUND:
      break;
  }

  
}



void setup() {
  Serial.begin(9600);

}

void loop() {

}
