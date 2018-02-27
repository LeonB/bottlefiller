#include "FillReport.h"
#include "BottleType.h"

void FillReport::Reset()
{
    this->stopWatch.reset();
    this->bottleWeight = 0.0;
    this->bottleType = UNKNOWN_BOTTLE;
    this->fillWeight = 0.0;
    this->scaleOffset = 0.0;
}
