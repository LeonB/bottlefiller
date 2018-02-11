// set some program constants
const unsigned short int DEFAULT_SCALE_MEASUREMENTS_PER_SECOND = 11;
const unsigned int DEFAULT_BOTTLE_DEVIATION = 1000;
const unsigned int MIN_WEIGHT_DIFF_TO_REGISTER = 100;

// pin assignments
const int HX711_DOUT = A1;
const int HX711_SCK = A0;

#include "ArduinoLog.h"
#include "BottleType.h"
#include "Scale.h"
#include "StopWatch.h"
#include "MemoryFree.h"

// declare scale
Scale scale;

// declare stopwatch for measuring stuff
StopWatch sw;

// declare list of bottle types
BottleType bottleTypes[10];

double weight;
bool weightIsChanging = false;

void printNewline(Print* _logOutput) {
    _logOutput->print("\r\n");
}

void initSerial()
{
    Serial.begin(9600);
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
    Log.notice("initScale");
    scale = Scale(HX711_DOUT, HX711_SCK);
}

void loadBottles(BottleType bottleTypes[10])
{
    Log.notice("loadBottles");
    bottleTypes[0].Name = "trappist";
    bottleTypes[0].MinWeight = 105300;
    bottleTypes[0].MaxWeight = 107300;

    bottleTypes[1].Name = "steinie";
    bottleTypes[1].MinWeight = 93450;
    bottleTypes[1].MaxWeight = 95450;

    bottleTypes[2].Name = "longneck";
    bottleTypes[2].MinWeight = 68000;
    bottleTypes[2].MaxWeight = 70000;

    bottleTypes[3].Name = "vichy 33 cl";
    bottleTypes[3].MinWeight = 77800;
    bottleTypes[3].MaxWeight = 79800;

    bottleTypes[4].Name = "bnr";
    bottleTypes[4].MinWeight = 81500;
    bottleTypes[4].MaxWeight = 83500;

    bottleTypes[5].Name = "steinie met beugel";
    bottleTypes[5].MinWeight = 108000;
    bottleTypes[5].MaxWeight = 110000;
}

void setup()
{
    initSerial();
    initLogger();
    initScale();
    loadBottles(bottleTypes);
}

BottleType getBottleBasedOnWeight(double weight, BottleType bottleTypes[10]) {
    for (int i = 0; i < 10; i++) {
        BottleType bottleType = bottleTypes[i];
        if (weight > bottleType.MinWeight && weight < bottleType.MaxWeight) {
            return bottleType;
        }
    }

    return BottleType();
}

void loop()
{
    struct Update update = scale.Update();

    if (update.StableWeightUpdated) {
        Log.notice(F("New fast equilibrium"));
        Log.notice(F("New weight: %d"), update.StableWeight);

        if (update.WeightIsRemoved) {
            Log.notice(F("Weight is removed (accurate)"));
        }

        if (update.WeightIsPlaced) {
            Log.notice(F("Weight is placed (accurate)"));
            BottleType bottleType = getBottleBasedOnWeight(update.Weight, bottleTypes);
            Log.notice(F("Bottle type: %s"), bottleType.Name.c_str());
        }

        Log.notice(F("-------------------------------"));
    }
}
