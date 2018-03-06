BOARD_TAG        = uno
MONITOR_CMD      = screen -L
MONITOR_BAUDRATE = 19200
ARDUINO_DIR      = $(HOME)/bin/arduino-1.8.5/
USER_LIB_PATH    = $(realpath libraries)
ARDUINO_LIB_PATH = $(realpath ../../libraries)
ARDUINO_LIBS     = HX711 \
				   StopWatch \
				   Chrono \
				   RunningMedian \
				   MemoryFree \
				   Arduino-Log \
				   MultiButton
TAGS_FILE        = ctags

include Arduino.mk

fmt: $(LOCAL_SRCS)
	astyle --style=kr $(ASTYLEFLAGS) $^

# vim: syntax=make ts=4 sw=4 sts=4 sr noet
