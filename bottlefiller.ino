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
    strcpy(bottleTypes[0].Name, "westmalle");
    bottleTypes[0].MinWeight = 105300;
    bottleTypes[0].MaxWeight = 107300;

    strcpy(bottleTypes[1].Name, "duvel");
    bottleTypes[1].MinWeight = 93450;
    bottleTypes[1].MaxWeight = 95450;

    strcpy(bottleTypes[2].Name, "longneck");
    bottleTypes[2].MinWeight = 68000;
    bottleTypes[2].MaxWeight = 70000;

    strcpy(bottleTypes[3].Name, "belgian");
    bottleTypes[3].MinWeight = 77800;
    bottleTypes[3].MaxWeight = 79800;

    strcpy(bottleTypes[4].Name, "bnr");
    bottleTypes[4].MinWeight = 81500;
    bottleTypes[4].MaxWeight = 83500;
}

void setup()
{
    initSerial();
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
    bool updated = scale.Update();

    if (updated && scale.NewStableWeightFast()) {
        Serial.println("New fast equilibrium");
        Serial.print("New weight: ");
        Serial.println(scale.GetFastValue());

        if (scale.WeightIsRemovedFast()) {
            Serial.println("Weight is removed (fast)");
        }

        if (scale.WeightIsPlacedFast()) {
            Serial.println("Weight is placed (fast)");
            BottleType bottleType = getBottleBasedOnWeight(scale.GetFastValue(), bottleTypes);
            Serial.print("Bottle type: ");
            Serial.println(bottleType.Name);
        }

        Serial.println("-------------------------------");
    }

    if (updated && scale.NewStableWeightAccurate()) {
        Serial.println("New accurate equilibrium");
        Serial.print("New weight: ");
        Serial.println(scale.GetAccurateValue());

        if (scale.WeightIsRemovedAccurate()) {
            Serial.println("Weight is removed (accurate)");
        }

        if (scale.WeightIsPlacedAccurate()) {
            Serial.println("Weight is placed (accurate)");
            BottleType bottleType = getBottleBasedOnWeight(scale.GetAccurateValue(), bottleTypes);
            Serial.print("Bottle type: ");
            Serial.println(bottleType.Name);
        }

        Serial.println("-------------------------------");

        if (scale.WeightIsRemovedAccurate()) {
            /* scale.Tare(); */
            Serial.println("-------------------------------");
        }
    }

    /* double newWeight = weight; */
    /* bool newWeightIsChanging = weightIsChanging; */

    /* if ( && scale.GetWeightIsStableFast()) { */
    /* if scale.State().Diff(scalesate) { */
    /* } */


    /* if (scale.NewEquilibrium()) { */
    /*     Serial.println("Stable"); */
    /* } */

    /* } */
    /* Serial.println("--------------------"); */
    /* newWeight = scale.GetFastValue(); */
    /* double weightDiff = abs(newWeight - weight); */
    /* if (weightDiff > MIN_WEIGHT_DIFF_TO_REGISTER) { */
    /*     newWeightIsChanging = true; */
    /* } else { */
    /*     newWeightIsChanging = false; */
    /* } */

    /* if (weightIsChanging == true && newWeightIsChanging == false) { */
    /*     Serial.print("New weight: "); */
    /*     Serial.println(newWeight); */
    /* } */

    /* if (weightIsChanging == true && newWeightIsChanging == false) { */
    /* if (weightIsChanging != newWeightIsChanging) { */
    /* Serial.println("-------------------"); */
    /* Serial.print("weightIsChanging: "); */
    /* Serial.println(weightIsChanging); */
    /* Serial.print("newWeightIsChanging: "); */
    /* Serial.println(newWeightIsChanging); */
    /* Serial.print("weightDiff: "); */
    /* Serial.println(weightDiff); */
    /* Serial.print("newWeight: "); */
    /* Serial.println(newWeight); */
    /* } */
    /* } */

    /* weight = newWeight; */
    /* weightIsChanging = newWeightIsChanging; */
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
