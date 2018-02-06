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
    Scale();
    Scale(int, int);
    bool Update();
    void Tare();
    double GetAccurateValue();
    double GetFastValue();
    bool GetWeightIsStableFast();
    bool GetWeightIsStableAccurate();
    /* void GetLastThreeValues(double[3]); */
    bool NewFastEquilibrium();
    bool NewAccurateEquilibrium();

private:
    // chrone is used for keeping track when measurements have to be taken
    Chrono chrono;
    // measurementsPerSecond is used for determining when updates should
    // hapen
    unsigned short int measurementsPerSecond;
    HX711 loadCell;
    RunningMedian accurateAverage;
    RunningMedian fastAverage;
    double weight;
    bool weightIsStableFast;
    bool weightIsStableAccurate;
    bool calculateIfWeightIsStableFast();
    bool calculateIfWeightIsStableAccurate();
    bool newFastEquilibrium;
    bool newAccurateEquilibrium;
};
