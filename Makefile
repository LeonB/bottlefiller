BOARD_TAG        = uno
ARDUINO_DIR      = $(HOME)/bin/arduino-1.8.5/
USER_LIB_PATH    = $(realpath libraries)
ARDUINO_LIB_PATH = $(realpath ../../libraries)
ARDUINO_LIBS     =

include Arduino.mk
