#include <WString.h>

#ifndef BOTTLE_TYPE
#define BOTTLE_TYPE

class BottleType
{
public:
    String Name;
    unsigned long MinWeight, MaxWeight, LiquidWeight;

    BottleType();
    BottleType(String);
    bool operator==(const BottleType) const;
    bool operator!=(const BottleType) const;
};

const BottleType UNKNOWN_BOTTLE PROGMEM = BottleType("unknown");
const int MAX_BOTTLE_TYPES = 6;

BottleType getBottleBasedOnWeight(long, BottleType[MAX_BOTTLE_TYPES]);
void loadBottles(BottleType bottleTypes[MAX_BOTTLE_TYPES]);

#endif
