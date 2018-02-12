#include <WString.h>

class BottleType
{
    public:
    String Name;
    unsigned long MinWeight, MaxWeight;

    BottleType();
    BottleType(String);
};

const BottleType UNKNOWN_BOTTLE PROGMEM = BottleType("unknown");
