#include "BottleType.h"
#include "ArduinoLog.h"

BottleType getBottleBasedOnWeight(long weight, BottleType bottleTypes[MAX_BOTTLE_TYPES])
{
    for (int i = 0; i < MAX_BOTTLE_TYPES; i++) {
        BottleType bottleType = bottleTypes[i];

        if (strlen(bottleType.Name) == 0) {
            continue;
        }

        if (weight > (signed long)bottleType.MinWeight && weight < (signed long)bottleType.MaxWeight) {
            return bottleType;
        }
    }

    return UNKNOWN_BOTTLE;
}

void loadBottles(BottleType bottleTypes[MAX_BOTTLE_TYPES])
{
    Log.notice(F("loadBottles"));
    strcpy(bottleTypes[0].Name, "trappist");
    /* bottleTypes[0].Name = F("trappist"); */
    bottleTypes[0].MinWeight = 102000;
    bottleTypes[0].MaxWeight = 107500;
    bottleTypes[0].LiquidWeight = 107000/5;

    strcpy(bottleTypes[1].Name, "steinie");
    /* bottleTypes[1].Name = F("steinie"); */
    bottleTypes[1].MinWeight = 93000;
    bottleTypes[1].MaxWeight = 96000;
    bottleTypes[1].LiquidWeight = 107000;


    strcpy(bottleTypes[2].Name, "longneck");
    /* bottleTypes[2].Name = F("longneck"); */
    bottleTypes[2].MinWeight = 67000;
    bottleTypes[2].MaxWeight = 70000;
    bottleTypes[2].LiquidWeight = 107000;


    strcpy(bottleTypes[3].Name, "vichy 33 cl");
    /* bottleTypes[3].Name = F("vichy 33 cl"); */
    bottleTypes[3].MinWeight = 77000;
    bottleTypes[3].MaxWeight = 80000;
    bottleTypes[3].LiquidWeight = 107000;


    strcpy(bottleTypes[4].Name, "bnr");
    /* bottleTypes[4].Name = F("bnr"); */
    bottleTypes[4].MinWeight = 81000;
    bottleTypes[4].MaxWeight = 84000;
    bottleTypes[4].LiquidWeight = 107000;

    strcpy(bottleTypes[5].Name, "steinie met beugel");
    /* bottleTypes[5].Name = F("steinie met beugel"); */
    bottleTypes[5].MinWeight = 108000;
    bottleTypes[5].MaxWeight = 112000;
    bottleTypes[5].LiquidWeight = 107000;
}

BottleType::BottleType()
{
}

BottleType::BottleType(char name[])
{
    strcpy(this->Name, Name);
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
