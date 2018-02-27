#include "BottleType.h"
#include "StopWatch.h"

#ifndef FILL_REPORT
#define FILL_REPORT

class FillReport
{
public:
    void Reset();
    StopWatch stopWatch;
    long bottleWeight;
    BottleType bottleType;
    long fullWeight;
    /* ScaleMeasurements; */
    long scaleOffset;
};

#endif
