#include "BottleType.h"

BottleType getBottleBasedOnWeight(double weight, BottleType bottleTypes[MAX_BOTTLE_TYPES])
{
    for (int i = 0; i < MAX_BOTTLE_TYPES; i++) {
        BottleType bottleType = bottleTypes[i];
        if (weight > bottleType.MinWeight && weight < bottleType.MaxWeight) {
            return bottleType;
        }
    }

    return UNKNOWN_BOTTLE;
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
