#include <TimerOne.h>
#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>
#include <Encoder.h>
#include <Keyboard.h>


#define LEFT_KNOB_A 0
#define LEFT_KNOB_B 1
#define LEFT_KNOB_BUTTON 8

#define RIGHT_KNOB_A 2
#define RIGHT_KNOB_B 3
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


void leftButtonInterrupt() {
  uint8_t trigger = getPCINTTrigger(digitalPinToPCINT(LEFT_KNOB_BUTTON));

  if (trigger == FALLING) {
    unsigned long left_release_interval;
    if (left_release_time > millis()) {//there was a rollover
      left_release_interval = (4294967295 - left_release_time) + millis();
    } else {
      left_release_interval = millis() - left_release_time;
    }
    
    if (left_release_interval < DOUBLE_PRESS_MILLIS) {
      left_button = DOUBLE_PRESS;
    }
    else {
      Timer1.setPeriod(LONG_PRESS_MILLIS * 1000);
      Timer1.attachInterrupt(leftButtonLongPressTimer);
      Timer1.start();
    }
  }
  else  if (trigger == RISING) {
    if (! left_long_pressed) {
      left_release_time = millis();
      Timer1.detachInterrupt();
      Timer1.setPeriod(DOUBLE_PRESS_MILLIS * 1000);
      Timer1.attachInterrupt(leftButtonShortPressTimer);
      Timer1.start();
    }
  }    
}

void leftButtonShortPressTimer() {
  Timer1.detachInterrupt();
  left_button = SHORT_PRESS;
}

void leftButtonLongPressTimer() {
  Timer1.detachInterrupt();
  left_long_pressed = true;
  left_button = LONG_PRESS;
}

void rightButtonInterrupt() {
  uint8_t trigger = getPCINTTrigger(digitalPinToPCINT(RIGHT_KNOB_BUTTON));

  if (trigger == FALLING) {
    unsigned long right_release_interval;
    if (right_release_time > millis()) {//there was a rollover
      right_release_interval = (4294967295 - right_release_time) + millis();
    } else {
      right_release_interval = millis() - right_release_time;
    }
    
    if (right_release_interval < DOUBLE_PRESS_MILLIS) {
      right_button = DOUBLE_PRESS;
    }
    else {
      Timer1.setPeriod(LONG_PRESS_MILLIS * 1000);
      Timer1.attachInterrupt(rightButtonLongPressTimer);
      Timer1.start();
    }
  }
  else  if (trigger == RISING) {
    if (! right_long_pressed) {
      right_release_time = millis();
      Timer1.detachInterrupt();
      Timer1.setPeriod(DOUBLE_PRESS_MILLIS * 1000);
      Timer1.attachInterrupt(rightButtonShortPressTimer);
      Timer1.start();
    }
  }    
}

void rightButtonShortPressTimer() {
  Timer1.detachInterrupt();
  right_button = SHORT_PRESS;
}

void rightButtonLongPressTimer() {
  Timer1.detachInterrupt();
  right_long_pressed = true;
  right_button = LONG_PRESS;
}

Encoder knobLeft(LEFT_KNOB_A, LEFT_KNOB_B);
Encoder knobRight(RIGHT_KNOB_A, RIGHT_KNOB_B);

void setup() {
  //pinMode(LEFT_KNOB_A, INPUT_PULLUP);
  //pinMode(LEFT_KNOB_B, INPUT_PULLUP);
  pinMode(LEFT_KNOB_BUTTON, INPUT_PULLUP);
  //pinMode(RIGHT_KNOB_A, INPUT_PULLUP);
  //pinMode(RIGHT_KNOB_B, INPUT_PULLUP);
  pinMode(RIGHT_KNOB_BUTTON, INPUT_PULLUP);

  //interrupts for buttons
  attachPCINT(digitalPinToPCINT(LEFT_KNOB_BUTTON), leftButtonInterrupt, CHANGE);
  attachPCINT(digitalPinToPCINT(RIGHT_KNOB_BUTTON), rightButtonInterrupt, CHANGE);

  //.5 second timer for long press, in microseconds
  Timer1.initialize(LONG_PRESS_MILLIS * 1000);
}

void loop() {
  int left_knob, right_knob;
  
  left_knob = knobLeft.read();
  knobLeft.write(0);
  if (left_knob > 0) {
    for (int i = 0; i < left_knob / 4; i++) {
      Keyboard.write(KEY_RIGHT_ARROW);
    }
  }
  else if (left_knob < 0) {
    for (int i = 0; i > left_knob / 4; i--) {
      Keyboard.write(KEY_LEFT_ARROW);
    }
  }
  
  right_knob = knobRight.read();
  knobRight.write(0);
  if (right_knob > 0) {
    for (int i = 0; i < right_knob / 4; i++) {
      Keyboard.write(KEY_UP_ARROW);
    }
  }
  else if (right_knob < 0) {
    for (int i = 0; i > right_knob / 4; i--) {
      Keyboard.write(KEY_DOWN_ARROW);
    }
  }

  switch(left_button) {
    case SHORT_PRESS:
      Keyboard.write(KEY_F1);
      break;

    case LONG_PRESS:
      Keyboard.write(KEY_F2);
      break;

    case DOUBLE_PRESS:
      Keyboard.write(KEY_F3);
      break;
  }
  left_button = UNPRESSED;

  switch(right_button) {
    case SHORT_PRESS:
      Keyboard.write(KEY_F4);
      break;

    case LONG_PRESS:
      Keyboard.write(KEY_F5);
      break;

    case DOUBLE_PRESS:
      Keyboard.write(KEY_F6);
      break;
  }
  right_button = UNPRESSED;
}
