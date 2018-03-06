#ifndef SCALE_UPDATE
#define SCALE_UPDATE

class ScaleUpdate
{
public:
    long OldWeight;
    long Weight;
    long OldStableWeight;
    long StableWeight;
    bool WeightIsRemoved;
    bool WeightIsPlaced;
    long WeightDiff;
    long StableWeightDiff;
    bool OldWeightIsStable;
    bool WeightIsStable;
    bool StableWeightUpdated;
    bool WeightUpdated;
};

#endif
