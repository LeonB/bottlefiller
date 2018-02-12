#include <Chrono.h>
#include "HX711.h"
#include "RunningMedian.h"
#include "ScaleUpdate.h"

class Scale
{
public:
    Scale();
    Scale(int, int);
    ScaleUpdate Update();
    void Tare();
    void SetOffset(long);
    void UpdateOffset(long);
    long GetOffset();
    void SetMeasurementsPerSecond(int);
    void SetMaxWeightDiffToBeStable(unsigned int);
private:
    // chrone is used for keeping track when measurements have to be taken
    Chrono chrono;
    // measurementsPerSecond is used for determining when updates should happen
    unsigned short int measurementsPerSecond = 10;
    unsigned int maxWeightDiffToBeStable = 100;
    HX711 loadCell;
    RunningMedian fastAverage();
    RunningMedian average;
    double calculateWeightFast();
    double calculateWeightAccurate();
    double weightFast,weightAccurate;
    double stableWeightFast,stableWeightAccurate;

    ScaleUpdate updateStatusFast(ScaleUpdate);
    ScaleUpdate updateStatusAccurate(ScaleUpdate);
    ScaleUpdate updateStatus(ScaleUpdate);
    bool calculateIfWeightIsStableFast();
    bool calculateIfWeightIsStableAccurate();
    RunningMedian updateFastAverageWithDiff(long);
    RunningMedian updateAccurateAverageWithDiff(long);
    RunningMedian updateAverageWithDiff(RunningMedian, long);
};
