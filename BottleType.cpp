#include "BottleType.h"
#include "ArduinoLog.h"

BottleType getBottleBasedOnWeight(int weight, BottleType bottleTypes[MAX_BOTTLE_TYPES])
{
    for (int i = 0; i < MAX_BOTTLE_TYPES; i++) {
        BottleType bottleType = bottleTypes[i];

        if (bottleType.Name == "") {
            continue;
        }

        if (weight > (signed int)bottleType.MinWeight && weight < (signed int)bottleType.MaxWeight) {
            return bottleType;
        }
    }

    return UNKNOWN_BOTTLE;
}

void loadBottles(BottleType bottleTypes[MAX_BOTTLE_TYPES])
{
    Log.notice(F("loadBottles"));
    bottleTypes[0].Name = F("trappist");
    bottleTypes[0].MinWeight = unsigned(102000);
    bottleTypes[0].MaxWeight = unsigned(107500);
    bottleTypes[0].LiquidWeight = unsigned(round(107000/5));

    bottleTypes[1].Name = F("steinie");
    bottleTypes[1].MinWeight = unsigned(93000);
    bottleTypes[1].MaxWeight = unsigned(96000);
    bottleTypes[1].LiquidWeight = unsigned(107000);


    bottleTypes[2].Name = F("longneck");
    bottleTypes[2].MinWeight = unsigned(67000);
    bottleTypes[2].MaxWeight = unsigned(70000);
    bottleTypes[2].LiquidWeight = unsigned(107000);


    bottleTypes[3].Name = F("vichy 33 cl");
    bottleTypes[3].MinWeight = unsigned(77000);
    bottleTypes[3].MaxWeight = unsigned(80000);
    bottleTypes[3].LiquidWeight = unsigned(107000);


    bottleTypes[4].Name = F("bnr");
    bottleTypes[4].MinWeight = unsigned(81000);
    bottleTypes[4].MaxWeight = unsigned(84000);
    bottleTypes[4].LiquidWeight = unsigned(107000);

    bottleTypes[5].Name = F("steinie met beugel");
    bottleTypes[5].MinWeight = unsigned(108000);
    bottleTypes[5].MaxWeight = unsigned(112000);
    bottleTypes[5].LiquidWeight = unsigned(107000);
}

BottleType::BottleType()
{
}

BottleType::BottleType(String name)
{
    this->Name = name;
}

bool BottleType::operator==(const BottleType other) const
{
    if (this->Name == other.Name) {
        return true;
    }

    return false;
}

bool BottleType::operator!=(const BottleType other) const
{
    return !(*this == other);
}
