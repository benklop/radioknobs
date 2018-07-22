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

void leftButtonInterrupt() {
  uint8_t trigger = getPCINTTrigger(digitalPinToPCINT(LEFT_KNOB_BUTTON));

  if (trigger == FALLING) {
    Keyboard.press(KEY_F1);
  }
  else  if (trigger == RISING) {
    Keyboard.release(KEY_F1);
  }    
}

void rightButtonInterrupt() {
  uint8_t trigger = getPCINTTrigger(digitalPinToPCINT(RIGHT_KNOB_BUTTON));

  if (trigger == FALLING) {
    Keyboard.press(KEY_F2);
  }
  else  if (trigger == RISING) {
    Keyboard.release(KEY_F2);
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
}
