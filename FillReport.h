#include "BottleType.h"
#include "StopWatch.h"

#ifndef FILL_REPORT
#define FILL_REPORT

class FillReport
{
public:
    void Reset();
    StopWatch stopWatch;
    double bottleWeight;
    BottleType bottleType;
    double fillWeight;
    /* ScaleMeasurements; */
    long scaleOffset;
};

#endif
