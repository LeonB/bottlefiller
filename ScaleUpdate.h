#ifndef SCALE_UPDATE
#define SCALE_UPDATE

class ScaleUpdate
{
public:
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

#endif
