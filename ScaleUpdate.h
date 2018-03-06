#ifndef SCALE_UPDATE
#define SCALE_UPDATE

class ScaleUpdate
{
public:
    int OldWeight;
    int Weight;
    int OldStableWeight;
    int StableWeight;
    bool WeightIsRemoved;
    bool WeightIsPlaced;
    int WeightDiff;
    int StableWeightDiff;
    bool OldWeightIsStable;
    bool WeightIsStable;
    bool StableWeightUpdated;
    bool WeightUpdated;
};

#endif
