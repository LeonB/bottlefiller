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
    void SetOffset(int);
    void UpdateOffset(int);
    int GetOffset();
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
    HX711 loadCell;
    RunningMedian fastAverage();
    RunningMedian average;
    int calculateWeightFast();
    int calculateWeightAccurate();
    int weightFast,weightAccurate;
    int stableWeightFast,stableWeightAccurate;

    ScaleUpdate updateStatusFast(ScaleUpdate);
    ScaleUpdate updateStatusAccurate(ScaleUpdate);
    ScaleUpdate updateStatus(ScaleUpdate);
    bool calculateIfWeightIsStableFast();
    bool calculateIfWeightIsStableAccurate();
    RunningMedian updateFastAverageWithDiff(int);
    RunningMedian updateAccurateAverageWithDiff(int);
    RunningMedian updateAverageWithDiff(RunningMedian, int);
};

#endif
