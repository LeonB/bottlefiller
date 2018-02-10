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
    struct Update Update();
    void Tare();
    void SetOffset(long);
    void UpdateOffset(long);
    long GetOffset();

private:
    // chrone is used for keeping track when measurements have to be taken
    Chrono chrono;
    // measurementsPerSecond is used for determining when updates should
    // hapen
    unsigned short int measurementsPerSecond;
    HX711 loadCell;
    RunningMedian fastAverage();
    RunningMedian average;
    double calculateWeightFast();
    double calculateWeightAccurate();
    double weightFast,weightAccurate;
    double stableWeightFast,stableWeightAccurate;

    struct Update updateStatusFast(struct Update);
    struct Update updateStatusAccurate(struct Update);
    struct Update updateStatus(struct Update);
    bool calculateIfWeightIsStableFast();
    bool calculateIfWeightIsStableAccurate();
    RunningMedian updateFastAverageWithDiff(long);
    RunningMedian updateAccurateAverageWithDiff(long);
    RunningMedian updateAverageWithDiff(RunningMedian, long);
};

struct Update {
    double OldWeight;
    double Weight;
    double OldStableWeight;
    double StableWeight;
    bool WeightIsRemoved;
    bool WeightIsPlaced;
    double WeightDiff;
    bool OldWeightIsStable;
    bool WeightIsStable;
    bool StableWeightUpdated;
    bool AverageWeightUpdated;
};
