#include <LiquidCrystal.h>

//value used for analogRead()
int high_vol = 800;

//pins used for LEDs
int blue_led_in = A0;
int yellow_led_in = A1;
int red_led_in = A2;
int white_led_in = A3;

int blue_led = 2;
int yellow_led = 3;
int red_led = 4;
int white_led = 5;
int start_btn = A4;
int green_led_out = A5;

//pins used for lcd
int lcd_volt = 6;
int rs = 7;
int en = 8;
int d4 = 9;
int d5 = 10;
int d6 = 11;
int d7 = 12;
int contrast = 100;

//game states
enum {
  STATE_IDLE = 0,
  STATE_COUNTDOWN = 1,
  STATE_CHOOSE_PATTERN = 2,
  STATE_PLAY = 3,
  STATE_WIN = 4,
  STATE_LOSE = 5
};

//LEDs
enum {
  BLUE = 0,
  YELLOW,
  RED,
  WHITE
};

//game variables
int score = 0;
int State = STATE_IDLE; //default game state
int pattern[50];
int index = 0;
int debounce_time = 50;
bool btn_press = false;

//create lcd object
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void StateMachine() {
  switch (State) {
    case STATE_IDLE:
      lcd.clear();
      lcd.print("Press start");
      lcd.setCursor(0, 1);
      lcd.print("button");
      
      //flash LEDs
      if (digitalRead(green_led_out) == HIGH) {
        delay(500);
        digitalWrite(green_led_out, LOW);
      } else {
        delay(500);
        digitalWrite(green_led_out, HIGH);
      }

      //state transition
      if (analogRead(start_btn) > high_vol) {
        randomSeed(analogRead(start_btn));
        State = STATE_COUNTDOWN; 
      }
      break;
    case STATE_COUNTDOWN:
      //keep green LED on
      digitalWrite(green_led_out, HIGH);
      
      lcd.clear();
      delay(50);
      lcd.print("3");
      delay(1000);
      
      lcd.clear();
      lcd.print("2");
      
      delay(1000);
      lcd.clear();
      lcd.print("1");
      delay(1000);      

      State = STATE_CHOOSE_PATTERN;
      break;
    case STATE_CHOOSE_PATTERN:
      lcd.clear();
      //turn off green LED
      digitalWrite(green_led_out, LOW);
    
      //generate random pattern
      int i;
      for (i = 0; i < score + 1; i++) {
        pattern[i] = random(4);
        }
        
      //show pattern on hardware
      int j;
      j = 0;
      while (pattern[j] != -1) {
        if (pattern[j] == BLUE) {
          delay(400);
          digitalWrite(blue_led, HIGH);
          digitalWrite(yellow_led, LOW);
          digitalWrite(red_led, LOW);
          digitalWrite(white_led, LOW);
          delay(400);
          digitalWrite(blue_led, LOW);
          digitalWrite(yellow_led, LOW);
          digitalWrite(red_led, LOW);
          digitalWrite(white_led, LOW); 
        }
        else if (pattern[j] == YELLOW) {
          delay(400);
          digitalWrite(blue_led, LOW);
          digitalWrite(yellow_led, HIGH);
          digitalWrite(red_led, LOW);
          digitalWrite(white_led, LOW);
          delay(400);
          digitalWrite(blue_led, LOW);
          digitalWrite(yellow_led, LOW);
          digitalWrite(red_led, LOW);
          digitalWrite(white_led, LOW); 
        }
        else if (pattern[j] == RED) {
          delay(400);
          digitalWrite(blue_led, LOW);
          digitalWrite(yellow_led, LOW);
          digitalWrite(red_led, HIGH);
          digitalWrite(white_led, LOW);
          delay(400);
          digitalWrite(blue_led, LOW);
          digitalWrite(yellow_led, LOW);
          digitalWrite(red_led, LOW);
          digitalWrite(white_led, LOW); 
        }
        else if (pattern[j] == WHITE) {
          delay(400);
          digitalWrite(blue_led, LOW);
          digitalWrite(yellow_led, LOW);
          digitalWrite(red_led, LOW);
          digitalWrite(white_led, HIGH);
          delay(400);
          digitalWrite(blue_led, LOW);
          digitalWrite(yellow_led, LOW);
          digitalWrite(red_led, LOW);
          digitalWrite(white_led, LOW); 
        }
        j++;
      }
      State = STATE_PLAY;
      delay(500);
      lcd.clear();
      break;
    case STATE_PLAY:
      //tell player to repeat the pattern
      lcd.setCursor(0, 0);
      lcd.print("Repeat pattern");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(String(score));

      delay(debounce_time);

      if (analogRead(blue_led_in) < high_vol && analogRead(yellow_led_in) < high_vol && 
      analogRead(red_led_in) < high_vol && analogRead(white_led_in) < high_vol) {
        btn_press = false;
      }
      
      if (pattern[index] == -1) {
        State = STATE_WIN;
      }
      
      else if (analogRead(blue_led_in) > high_vol && !btn_press) {
        if (BLUE != pattern[index]) {
          State = STATE_LOSE;
        } else {
          btn_press = true;
          index++;
          }
        } 
      else if (analogRead(yellow_led_in) > high_vol && !btn_press) {
        if (YELLOW != pattern[index]) {
          State = STATE_LOSE;
          } else {
            btn_press = true;
            index++;
          }
        }
      else if (analogRead(red_led_in) > high_vol && !btn_press) {
        if (RED != pattern[index]) {
          State = STATE_LOSE;
        } else {
          btn_press = true;
          index++;
          }
        }
      else if (analogRead(white_led_in) > high_vol && !btn_press) {
        if (WHITE != pattern[index]) {
          State = STATE_LOSE;
        } else {
          btn_press = true;
          index++;
          }
        }
      break;
    case STATE_WIN:
      score++;
      delay(500);
      lcd.clear();
      lcd.print("Correct.");
      lcd.setCursor(9, 1);
      lcd.print("+1");
      delay(500);
      index = 0;
      State = STATE_CHOOSE_PATTERN;
      break;
    case STATE_LOSE:
      delay(500);
      lcd.clear();
      lcd.print("Wrong guess.");
      lcd.setCursor(0, 1);
      lcd.print("You lose!");
      delay(1400);
      index = 0;
      score = 0;
      
      //setup patten array
      for (int i = 0; i < sizeof(pattern) / sizeof(int); i++) {
        pattern[i] = -1;
      }
      
      State = STATE_IDLE;
      break;
  }
}

void setup() {
  Serial.begin(9600);

  //setup LED pins
  pinMode(white_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(yellow_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  pinMode(blue_led_in, INPUT);
  pinMode(yellow_led_in, INPUT);
  pinMode(red_led_in, INPUT);
  pinMode(white_led_in, INPUT);
  pinMode(start_btn, INPUT);
  pinMode(green_led_out, OUTPUT);

  //set LEDs to low by default
  digitalWrite(white_led, LOW);
  digitalWrite(red_led, LOW);
  digitalWrite(yellow_led, LOW);
  digitalWrite(blue_led, LOW);
  digitalWrite(green_led_out, LOW);

  //LCD setup
  analogWrite(lcd_volt, contrast);
  lcd.begin(16, 2);

  //setup patten array
  for (int i = 0; i < sizeof(pattern) / sizeof(int); i++) {
    pattern[i] = -1;
  }
}

void loop() {
  if (analogRead(blue_led_in) > high_vol) {
        digitalWrite(blue_led, HIGH);
  } else if (analogRead(blue_led_in) < high_vol) {
    digitalWrite(blue_led, LOW);
  }

  if (analogRead(yellow_led_in) > high_vol) {
        digitalWrite(yellow_led, HIGH);
  } else if (analogRead(yellow_led_in) < high_vol) {
    digitalWrite(yellow_led, LOW);
  }

  if (analogRead(red_led_in) > high_vol) {
        digitalWrite(red_led, HIGH);
  } else if (analogRead(red_led_in) < high_vol) {
    digitalWrite(red_led, LOW);
  }

  if (analogRead(white_led_in) > high_vol) {
        digitalWrite(white_led, HIGH);
  } else if (analogRead(white_led_in) < high_vol) {
    digitalWrite(white_led, LOW);
  }
  
  StateMachine();
  Serial.println(index);
}
