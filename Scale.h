#include <Chrono.h>
#include "NBHX711.h"
#include "RunningMedian.h"
#include "ScaleUpdate.h"

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
    unsigned short int measurementsPerSecond = 10;
    unsigned int maxWeightDiffToBeStable = 100;
    unsigned int weightDiffToRegisterAsPlaced = 1000;
    NBHX711 loadCell;
    RunningMedian fastAverage();
    RunningMedian average;
    long calculateWeightFast();
    long calculateWeightAccurate();
    long weightFast,weightAccurate;
    long stableWeightFast,stableWeightAccurate;

    ScaleUpdate updateStatusFast(ScaleUpdate);
    ScaleUpdate updateStatusAccurate(ScaleUpdate);
    ScaleUpdate updateStatus(ScaleUpdate);
    bool calculateIfWeightIsStableFast();
    bool calculateIfWeightIsStableAccurate();
    RunningMedian updateFastAverageWithDiff(long);
    RunningMedian updateAccurateAverageWithDiff(long);
    RunningMedian updateAverageWithDiff(RunningMedian, long);
};

#endif
