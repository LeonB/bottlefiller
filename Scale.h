#include <Chrono.h>
#include "HX711.h"
#include "RunningMedian.h"

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
