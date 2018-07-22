#include <TimerOne.h>
#include <TimerThree.h>
#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>
#include <Encoder.h>
#include <Keyboard.h>


#define LEFT_KNOB_A 2
#define LEFT_KNOB_B 3
#define LEFT_KNOB_BUTTON 8

#define RIGHT_KNOB_A 1
#define RIGHT_KNOB_B 0
#define RIGHT_KNOB_BUTTON 9

#define UNPRESSED 0
#define SHORT_PRESS 1
#define LONG_PRESS 2
#define DOUBLE_PRESS 3

#define DOUBLE_PRESS_MILLIS 250
#define LONG_PRESS_MILLIS 500

volatile unsigned long left_release_time, right_release_time = 0;

volatile bool left_long_pressed, right_long_pressed = false;
volatile int left_button, right_button = UNPRESSED;

void leftButtonShortPressTimer() {
  Timer1.stop();
  left_button = SHORT_PRESS;
}

void leftButtonLongPressTimer() {
  Timer1.stop();
  left_long_pressed = true;
  left_button = LONG_PRESS;
}

void leftButtonInterrupt() {
  uint8_t trigger = getPCINTTrigger(digitalPinToPCINT(LEFT_KNOB_BUTTON));
  left_long_pressed = false;
  if (trigger == FALLING) {
    unsigned long left_release_interval;
    if (left_release_time > millis()) {//there was a rollover
      left_release_interval = (4294967295L - left_release_time) + millis();
    } else {
      left_release_interval = millis() - left_release_time;
    }
    
    if (left_release_interval < DOUBLE_PRESS_MILLIS) {
      left_button = DOUBLE_PRESS;
    }
    else {
      Timer1.stop();
      Timer1.setPeriod(500000);
      Timer1.attachInterrupt(leftButtonLongPressTimer);
      Timer1.restart();
    }
  }
  else  if (trigger == RISING) {
    left_release_time = millis();
    if (! left_long_pressed) {
      Timer1.stop();
      Timer1.setPeriod(250000);
      Timer1.attachInterrupt(leftButtonShortPressTimer);
      Timer1.restart();
    }
  }    
}

void rightButtonShortPressTimer() {
  Timer1.stop();
  right_button = SHORT_PRESS;
}

void rightButtonLongPressTimer() {
  Timer1.stop();
  right_long_pressed = true;
  right_button = LONG_PRESS;
}

void rightButtonInterrupt() {
  uint8_t trigger = getPCINTTrigger(digitalPinToPCINT(RIGHT_KNOB_BUTTON));

  if (trigger == FALLING) {
    unsigned long right_release_interval;
    if (right_release_time > millis()) {//there was a rollover
      right_release_interval = (4294967295L - right_release_time) + millis();
    } else {
      right_release_interval = millis() - right_release_time;
    }
    
    if (right_release_interval < DOUBLE_PRESS_MILLIS) {
      right_button = DOUBLE_PRESS;
    }
    else {
      Timer1.stop();
      Timer1.setPeriod(LONG_PRESS_MILLIS * 1000L);
      Timer1.attachInterrupt(rightButtonLongPressTimer);
      Timer1.restart();
    }
  }
  else  if (trigger == RISING) {
    right_release_time = millis();
    if (! right_long_pressed) {
      Timer1.stop();
      Timer1.setPeriod(DOUBLE_PRESS_MILLIS * 1000L);
      Timer1.attachInterrupt(rightButtonShortPressTimer);
      Timer1.restart();
    }
  }    
}

Encoder knobLeft(LEFT_KNOB_A, LEFT_KNOB_B);
Encoder knobRight(RIGHT_KNOB_A, RIGHT_KNOB_B);

void setup() {
  pinMode(LEFT_KNOB_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_KNOB_BUTTON, INPUT_PULLUP);

  //interrupts for buttons
  attachPCINT(digitalPinToPCINT(LEFT_KNOB_BUTTON), leftButtonInterrupt, CHANGE);
  attachPCINT(digitalPinToPCINT(RIGHT_KNOB_BUTTON), rightButtonInterrupt, CHANGE);

  //.5 second timer for long press, in microseconds
  Timer1.initialize(10000000);

  //debugging
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
}

void loop() {
  int left_knob, right_knob;
  
  left_knob = knobLeft.read();
  if (left_knob > 0 && left_knob % 4 == 0) {
    for (int i = 0; i < left_knob / 4; i++) {
      Keyboard.write(KEY_RIGHT_ARROW);
      #ifdef DEBUG
        Serial.print("Left Knob UP");
        Serial.println();
      #endif
    }
    knobLeft.write(0);
  }
  else if (left_knob < 0 && left_knob % 4 == 0) {
    for (int i = 0; i > left_knob / 4; i--) {
      Keyboard.write(KEY_LEFT_ARROW);
      #ifdef DEBUG
        Serial.print("Left Knob DOWN");
        Serial.println();
      #endif
    }
    knobLeft.write(0);
  }
  
  right_knob = knobRight.read();
  if (right_knob > 0 && right_knob % 4 == 0) {
    for (int i = 0; i < right_knob / 4; i++) {
      Keyboard.write(KEY_UP_ARROW);
      #ifdef DEBUG
        Serial.print("Right Knob UP");
        Serial.println();
      #endif
    }
    knobRight.write(0);
  }
  else if (right_knob < 0 && right_knob % 4 == 0) {
    for (int i = 0; i > right_knob / 4; i--) {
      Keyboard.write(KEY_DOWN_ARROW);
      #ifdef DEBUG
        Serial.print("Right Knob DOWN");
        Serial.println();
      #endif
    }
    knobRight.write(0);
  }

  switch(left_button) {
    case SHORT_PRESS:
      Keyboard.write(KEY_F1);
      left_button = UNPRESSED;
      break;

    case LONG_PRESS:
      Keyboard.write(KEY_F2);
      left_button = UNPRESSED;
      break;

    case DOUBLE_PRESS:
      Keyboard.write(KEY_F3);
      left_button = UNPRESSED;
      break;
  }

  switch(right_button) {
    case SHORT_PRESS:
      Keyboard.write(KEY_F4);
      right_button = UNPRESSED;
      break;

    case LONG_PRESS:
      Keyboard.write(KEY_F5);
      right_button = UNPRESSED;
      break;

    case DOUBLE_PRESS:
      Keyboard.write(KEY_F6);
      right_button = UNPRESSED;
      break;
  }
}
