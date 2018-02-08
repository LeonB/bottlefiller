#include <Chrono.h>
#include "HX711.h"
#include "RunningMedian.h"

// defaults
#ifndef DEFAULT_SCALE_MEASUREMENTS_PER_SECOND
#define unsigned short int DEFAULT_SCALE_MEASUREMENTS_PER_SECOND = 10;
#endif

class Scale
{
public:
    bool NewFastEquilibrium();
    bool NewAccurateEquilibrium();
    bool WeightIsRemovedFast();
    bool WeightIsRemovedAccurate();
    bool WeightIsPlacedFast();
    bool WeightIsPlacedAccurate();

    Scale();
    Scale(int, int);
    bool Update();
    void Tare();
    double GetAccurateValue();
    double GetFastValue();
    bool GetWeightIsStableFast();
    bool GetWeightIsStableAccurate();
    double GetStableWeightFast();
    double GetStableWeightAccurate();

private:
    // chrone is used for keeping track when measurements have to be taken
    Chrono chrono;
    // measurementsPerSecond is used for determining when updates should
    // hapen
    unsigned short int measurementsPerSecond;
    HX711 loadCell;
    RunningMedian accurateAverage, fastAverage;
    bool weightIsStableFast, weightIsStableAccurate;
    bool newFastEquilibrium, newAccurateEquilibrium;
    bool weightIsRemovedFast, weightIsRemovedAccurate;
    bool weightIsPlacedFast, weightIsPlacedAccurate;
    double stableWeightFast,stableWeightAccurate;

    bool calculateIfWeightIsStableFast();
    bool calculateIfWeightIsStableAccurate();
};
