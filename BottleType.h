#ifndef BOTTLE_TYPE
#define BOTTLE_TYPE

class BottleType
{
public:
    char Name[20];
    unsigned long MinWeight, MaxWeight, LiquidWeight;

    BottleType();
    BottleType(char[]);
    bool operator==(const BottleType) const;
    bool operator!=(const BottleType) const;
};

/* const BottleType UNKNOWN_BOTTLE PROGMEM = BottleType("Unknown); */
const BottleType UNKNOWN_BOTTLE = BottleType((char *)"Unknown");
const int MAX_BOTTLE_TYPES = 10;

BottleType getBottleBasedOnWeight(long, BottleType[MAX_BOTTLE_TYPES]);
void loadBottles(BottleType bottleTypes[MAX_BOTTLE_TYPES]);

#endif
