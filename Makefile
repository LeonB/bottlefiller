BOARD_TAG        = uno
ARDUINO_DIR      = $(HOME)/bin/arduino-1.8.5/
USER_LIB_PATH    = $(realpath libraries)
ARDUINO_LIB_PATH = $(realpath ../../libraries)
ARDUINO_LIBS     = HX711 ArduinoMenu_library

include Arduino.mk
