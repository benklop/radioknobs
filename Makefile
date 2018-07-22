ARDUINO_DIR         = /opt/arduino-1.8.5
ARDMK_DIR           =/opt/Arduino-Makefile
ARDUINO_SKETCHBOOK  = $(HOME)/Arduino
ARCHITECTURE        = avr
ALTERNATE_CORE      = sparkfun
BOARD_TAG           = promicro
BOARD_SUB	    = 16MHzatmega32U4
CPPFLAGS            = -DARDUINO_ARCH_AVR
ARDUINO_LIBS        = HID Encoder Keyboard PinChangeInterrupt TimerOne

include $(ARDMK_DIR)/Arduino.mk

force_reset:
	gpio -g write 23 0
	sleep .5
	gpio -g write 23 1
	sleep .5
	gpio -g write 23 0
	sleep .5
	gpio -g write 23 1
