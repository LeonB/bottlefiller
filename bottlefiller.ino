#include "ArduinoLog.h"
#include "Scale.h"
#include "Valve.h"
#include "StateMachine.h"
#include "PinButton.h"
/* #include "StopWatch.h" */
/* #include "MemoryFree.h" */

// set some library constants
const unsigned short int SCALE_MEASUREMENTS_PER_SECOND = 11;
const unsigned int SCALE_MAX_WEIGHT_DIFF_TO_BE_STABLE = 500;
const unsigned int SCALE_WEIGHT_DIFF_TO_REGISTER_AS_PLACED = 3000;
const unsigned int DEFAULT_BOTTLE_DEVIATION = 3000;

// set some program constants
const int BAUD_RATE = 19200;

// pin assignments
const int HX711_DOUT = A1;
const int HX711_SCK = A0;
const int VALVE_PIN = 2;
const int GREEN_BUTTON_PIN = 12;
const int RED_BUTTON_PIN = 13;

// declare scale
Scale scale;

// declare valve
Valve valve;

PinButton greenButton(GREEN_BUTTON_PIN);
PinButton redButton(RED_BUTTON_PIN);

// declare stopwatch for measuring stuff
/* StopWatch sw; */

double weight;
bool weightIsChanging = false;
StateMachine stateMachine;

void printNewline(Print* _logOutput)
{
    _logOutput->print("\r\n");
}

void initSerial()
{
    Serial.begin(BAUD_RATE);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
}

void initLogger()
{
    // Initialize with log level and log output
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    Log.setSuffix(printNewline);
}

void initScale()
{
    Log.notice(F("initScale"));
    scale = Scale(HX711_DOUT, HX711_SCK);
    scale.SetMeasurementsPerSecond(SCALE_MEASUREMENTS_PER_SECOND);
    scale.SetMaxWeightDiffToBeStable(SCALE_MAX_WEIGHT_DIFF_TO_BE_STABLE);
    scale.SetWeightDiffToRegisterAsPlaced(SCALE_WEIGHT_DIFF_TO_REGISTER_AS_PLACED);
}

void initValve()
{
    Log.notice(F("initValve"));
    valve = Valve(VALVE_PIN);
}

void initStateMachine()
{
    stateMachine = StateMachine(
                       scale,
                       valve,
                       greenButton,
                       redButton);
}

void setup()
{
    initSerial();
    initLogger();
    initScale();
    initValve();
    initStateMachine();
}

bool open = false;
void loop()
{
    stateMachine.Loop();
}
