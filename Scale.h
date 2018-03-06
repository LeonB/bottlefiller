#include "ScaleUpdate.h"
#include <Chrono.h>
#include <HX711.h>
#include <RunningMedian.h>

#ifndef SCALE
#define SCALE

class Scale
{
public:
    Scale();
    Scale(byte, byte);
    ScaleUpdate Update();
    void Tare();
    void SetOffset(long);
    void UpdateOffset(long);
    long GetOffset();
    void SetMeasurementsPerSecond(int);
    void SetMaxWeightDiffToBeStable(unsigned int);
    void SetWeightDiffToRegisterAsPlaced(unsigned int);
private:
    // chrone is used for keeping track when measurements have to be taken
    Chrono chrono;
    // measurementsPerSecond is used for determining when updates should happen
    short int measurementsPerSecond = 10;
    short int maxWeightDiffToBeStable = 100;
    int weightDiffToRegisterAsPlaced = 1000;
    HX711 loadCell;
    RunningMedian fastAverage();
    RunningMedian average;

    long weightFast, stableWeightFast;
    long calculateWeightFast();
    ScaleUpdate updateStatusFast(ScaleUpdate);
    ScaleUpdate updateStatus(ScaleUpdate);
    bool calculateIfWeightIsStableFast();
    RunningMedian updateFastAverageWithDiff(long);

    /* long weightAccurate, stableWeightAccurate; */
    /* long calculateWeightAccurate(); */
    /* long calculateWeightAccurate(); */
    /* bool calculateIfWeightIsStableAccurate(); */
    /* RunningMedian updateAccurateAverageWithDiff(long); */

    RunningMedian updateAverageWithDiff(RunningMedian, long);
};

#endif
