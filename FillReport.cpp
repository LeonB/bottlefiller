#include "FillReport.h"
#include "BottleType.h"

void FillReport::Reset()
{
    this->bottleWeight = 0.0;
    this->bottleType = UNKNOWN_BOTTLE;
    this->fullWeight = 0.0;
    this->scaleOffset = 0.0;
    this->stopWatch.reset();
    this->stopWatch.start();
}
