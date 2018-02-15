#include "ArduinoLog.h"
#include "BottleType.h"
#include "Scale.h"
#include "StateMachine.h"
/* #include "StopWatch.h" */
/* #include "MemoryFree.h" */

// set some library constants
const unsigned short int SCALE_MEASUREMENTS_PER_SECOND = 11;
const unsigned int SCALE_MAX_WEIGHT_DIFF_TO_BE_STABLE = 500;
const unsigned int SCALE_WEIGHT_DIFF_TO_REGISTER_AS_PLACED = 3000;
const unsigned int DEFAULT_BOTTLE_DEVIATION = 3000;

// set some program constants
const int BAUD_RATE = 9600;
// pin assignments
const int HX711_DOUT = A1;
const int HX711_SCK = A0;

// declare scale
Scale scale;

// declare stopwatch for measuring stuff
/* StopWatch sw; */

// declare list of bottle types
BottleType bottleTypes[MAX_BOTTLE_TYPES];

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

void initStateMachine()
{
    stateMachine = StateMachine(scale, bottleTypes);
}

void loadBottles(BottleType bottleTypes[MAX_BOTTLE_TYPES])
{
    Log.notice(F("loadBottles"));
    bottleTypes[0].Name = "trappist";
    bottleTypes[0].MinWeight = 106000;
    bottleTypes[0].MaxWeight = 108000;
    bottleTypes[0].LiquidWeight = 170600;

    bottleTypes[1].Name = "steinie";
    bottleTypes[1].MinWeight = 93000;
    bottleTypes[1].MaxWeight = 96000;
    bottleTypes[1].LiquidWeight = 170600;


    bottleTypes[2].Name = "longneck";
    bottleTypes[2].MinWeight = 67000;
    bottleTypes[2].MaxWeight = 70000;
    bottleTypes[2].LiquidWeight = 170600;


    bottleTypes[3].Name = "vichy 33 cl";
    bottleTypes[3].MinWeight = 77000;
    bottleTypes[3].MaxWeight = 80000;
    bottleTypes[3].LiquidWeight = 170600;


    bottleTypes[4].Name = "bnr";
    bottleTypes[4].MinWeight = 81000;
    bottleTypes[4].MaxWeight = 84000;
    bottleTypes[4].LiquidWeight = 170600;

    bottleTypes[5].Name = "steinie met beugel";
    bottleTypes[5].MinWeight = 108500;
    bottleTypes[5].MaxWeight = 111500;
    bottleTypes[5].LiquidWeight = 170600;
}

void setup()
{
    initSerial();
    initLogger();
    loadBottles(bottleTypes);
    initScale();
    initStateMachine();
}

void loop()
{
    stateMachine.Loop();
}
