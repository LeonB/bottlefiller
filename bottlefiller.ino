// set some program constants
const unsigned short int DEFAULT_SCALE_MEASUREMENTS_PER_SECOND = 11;
const unsigned int DEFAULT_BOTTLE_DEVIATION = 1000;
const unsigned int MIN_WEIGHT_DIFF_TO_REGISTER = 100;

// pin assignments
const int HX711_DOUT = A1;
const int HX711_SCK = A0;

#include "BottleType.h"
#include "Scale.h"
#include <StopWatch.h>

// declare scale
Scale scale;

// declare stopwatch for measuring stuff
StopWatch sw;

// declare list of bottle types
BottleType bottleTypes[10];

double weight;
bool weightIsChanging = false;

void initSerial()
{
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
}

void initScale()
{
    Serial.println("initScale");
    scale = Scale(HX711_DOUT, HX711_SCK);
}

void loadBottles(BottleType bottleTypes[10])
{
    Serial.println("loadBottles");
    strcpy(bottleTypes[0].Name, "Test");
    bottleTypes[0].MinWeight = 0;
    bottleTypes[0].MaxWeight = 123;
}

void setup()
{
    initSerial();
    initScale();
    loadBottles(bottleTypes);
}

void loop()
{
    double newWeight = weight;
    bool newWeightIsChanging = weightIsChanging;

    if (scale.Update()) {
        /* Serial.println("--------------------"); */
        newWeight = scale.GetFastValue();
        double weightDiff = abs(newWeight - weight);
        if (weightDiff > MIN_WEIGHT_DIFF_TO_REGISTER) {
            newWeightIsChanging = true;
        } else {
            newWeightIsChanging = false;
        }

        /* if (weightIsChanging == true && newWeightIsChanging == false) { */
        /*     Serial.print("New weight: "); */
        /*     Serial.println(newWeight); */
        /* } */

        if (weightIsChanging == true && newWeightIsChanging == false) {
        /* if (weightIsChanging != newWeightIsChanging) { */
            Serial.println("-------------------");
            Serial.print("weightIsChanging: ");
            Serial.println(weightIsChanging);
            Serial.print("newWeightIsChanging: ");
            Serial.println(newWeightIsChanging);
            Serial.print("weightDiff: ");
            Serial.println(weightDiff);
            Serial.print("newWeight: ");
            Serial.println(newWeight);
        }
    }

    weight = newWeight;
    weightIsChanging = newWeightIsChanging;
    /* Serial.println(bottleTypes[0].Name); */
    /* Serial.println(bottleTypes[0].MinWeight); */
    /* Serial.println(bottleTypes[0].MaxWeight); */
    /* sw.reset(); */
    /* sw.start(); */
    /* /1* Serial.println(scale.read()); *1/ */
    /* /1* Serial.println(scale.read_average(11)); *1/ */
    /* sw.stop(); */
    /* Serial.print("scale.read() took "); */
    /* Serial.print(sw.elapsed()); */
    /* Serial.println(" millis"); */
}
