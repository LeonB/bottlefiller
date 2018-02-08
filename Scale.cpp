#include "Scale.h"

Scale::Scale()
{
}

Scale::Scale(int pinDout, int pinSck)
{
    this->loadCell.begin(pinDout, pinSck);
    this->loadCell.set_offset(0.0);
    this->chrono.start();
    this->measurementsPerSecond = DEFAULT_SCALE_MEASUREMENTS_PER_SECOND;
    this->fastAverage = RunningMedian(3);
    this->accurateAverage = RunningMedian(DEFAULT_SCALE_MEASUREMENTS_PER_SECOND);

    Serial.println("Waiting for load cell to become ready");
    while (!this->loadCell.is_ready()) {
        ;
    }

    // setup zero weight scale
    this->Tare();
}

void Scale::Tare()
{
    // calculate how many new reading to take to fill average buffer
    uint8_t readingsToAdd = DEFAULT_SCALE_MEASUREMENTS_PER_SECOND - this->accurateAverage.getCount();
    Serial.print("Taking ");
    Serial.print(readingsToAdd);
    Serial.println(" new readings");

    // reset average
    this->fastAverage.clear();
    this->accurateAverage.clear();

    // take enough readings to get a proper median
    while (this->accurateAverage.getCount() < this->accurateAverage.getSize()) {
        bool updated = this->Update();
        if (updated) {
            Serial.println("New reading");
        }
    }

    Serial.println("Waiting for readings to stabilise");
    while (!this->GetWeightIsStableAccurate()) {
        bool updated = this->Update();
        if (updated) {
            Serial.println("New reading");
        }
    }

    // readings have stabilised: set new offset
    Serial.println("Readings have stabilised");
    this->SetOffset(this->GetAccurateValue());

    // set initial median as default offset
    Serial.print("Initial load cell offset: ");
    Serial.println(this->GetOffset());
}

bool Scale::Update()
{
    // test if at least 100ms (in the default case) have passed
    if (this->chrono.hasPassed(1000/this->measurementsPerSecond)) {
        // get old values
        double oldFastValue = this->GetFastValue();
        double oldAccurateValue = this->GetAccurateValue();
        bool oldStableFast = this->weightIsStableFast;
        bool oldStableAccurate = this->weightIsStableAccurate;
        double oldStableWeightFast = this->stableWeightFast;
        double oldStableWeightAccurate = this->stableWeightAccurate;

        // get new value
        double value = this->loadCell.get_value();

        // add reading to average
        this->accurateAverage.add(value);
        this->fastAverage.add(value);

        // restart chrono
        this->chrono.restart();

        // get new weights
        double newFastValue = this->GetFastValue();
        double newAccurateValue = this->GetAccurateValue();

        // calculate if weight has updated
        bool fastUpdated = oldFastValue != newFastValue;
        bool accurateUpdated = oldAccurateValue != newAccurateValue;

        // calculate if weight is stable
        this->weightIsStableFast = this->calculateIfWeightIsStableFast();
        this->weightIsStableAccurate = this->calculateIfWeightIsStableAccurate();

        if (oldStableFast == false && this->weightIsStableFast == true && !this->newFastEquilibrium) {
            this->newFastEquilibrium = true;

            Serial.print("oldStableWeightFast: ");
            Serial.println(oldStableWeightFast);
            Serial.print("newFastValue: ");
            Serial.println(newFastValue);

            if (oldStableWeightFast > newFastValue) {
                this->weightIsRemovedFast = true;
                this->weightIsPlacedFast = false;
                this->stableWeightFast = newFastValue;
            } else {
                this->weightIsRemovedFast = false;
                this->weightIsPlacedFast = true;
            }
        } else {
            this->newFastEquilibrium = false;
            this->weightIsPlacedFast = false;
            this->weightIsRemovedFast = false;
        }

        if (oldStableAccurate == false && this->weightIsStableAccurate == true && !this->newAccurateEquilibrium) {
            this->newAccurateEquilibrium = true;

            Serial.print("oldStableWeightAccurate: ");
            Serial.println(oldStableWeightAccurate);
            Serial.print("newAccurateValue: ");
            Serial.println(newAccurateValue);

            if (oldStableWeightAccurate > newAccurateValue) {
                this->weightIsRemovedAccurate = true;
                this->weightIsPlacedAccurate = false;
                this->stableWeightAccurate = newFastValue;
            } else {
                this->weightIsRemovedAccurate = false;
                this->weightIsPlacedAccurate = true;
            }
        } else {
            this->newAccurateEquilibrium = false;
            this->weightIsPlacedAccurate = false;
            this->weightIsRemovedAccurate = false;
        }

        return fastUpdated || accurateUpdated;
    }

    return false;
}

// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
double Scale::GetAccurateValue()
{
    /* return this->average.getAverage(2); */
    return this->accurateAverage.getMedian();
}

/* void Scale::GetLastThreeValues(double values[3]) */
/* { */
/*     uint8_t needed = 3; */
/*     uint8_t count = this->fastAverage.getCount(); */
/*     uint8_t use = count; */
/*     if (use > needed) { */
/*         use = needed; */
/*     } */

/*     RunningMedian fastAverage(needed); */
/*     for (int i = 0; i < use; ++i) { */
/*         double val = this->fastAverage.getElement(count -1 - i); */
/*         values[i] = val; */
/*     } */

/*     return; */
/* } */

/* RunningMedian Scale::GetFastAverage() */
/* { */
/*     uint8_t needed = 3; */
/*     uint8_t count = this->fastAverage.getCount(); */
/*     uint8_t use = count; */
/*     if (use > needed) { */
/*         use = needed; */
/*     } */

/*     RunningMedian fastAverage(needed); */
/*     for (int i = 0; i < use; ++i) { */
/*         double val = this->fastAverage.getElement(count -1 - i); */
/*         fastAverage.add(val); */
/*     } */

/*     return fastAverage; */
/* } */

double Scale::GetFastValue()
{
    return this->fastAverage.getMedian();
}

bool Scale::calculateIfWeightIsStableFast()
{
    double diff = this->fastAverage.getHighest() - this->fastAverage.getLowest();
    /* Serial.print("Fast diff: "); */
    /* Serial.println(diff); */
    return diff < 250;
}

bool Scale::calculateIfWeightIsStableAccurate()
{
    double diff = this->accurateAverage.getHighest() - this->accurateAverage.getLowest();
    /* Serial.print("Accurate diff: "); */
    /* Serial.println(diff); */
    return diff < 750;
}

bool Scale::GetWeightIsStableFast()
{
    return this->weightIsStableFast;
}

bool Scale::GetWeightIsStableAccurate()
{
    return this->weightIsStableAccurate;
}

bool Scale::NewFastEquilibrium()
{
    return this->newFastEquilibrium;
}

bool Scale::NewAccurateEquilibrium()
{
    return this->newAccurateEquilibrium;
}

bool Scale::WeightIsRemovedFast()
{
    return this->weightIsRemovedFast;
}

bool Scale::WeightIsRemovedAccurate()
{
    return this->weightIsRemovedAccurate;
}

bool Scale::WeightIsPlacedFast()
{
    return this->weightIsPlacedFast;
}

bool Scale::WeightIsPlacedAccurate()
{
    return this->weightIsPlacedAccurate;
}

double Scale::GetStableWeightFast()
{
    return this->stableWeightFast;
}

double Scale::GetStableWeightAccurate()
{
    return this->stableWeightAccurate;
}

void Scale::SetOffset(long offset) {
    long diff = this->GetOffset() - offset;
    this->loadCell.set_offset(offset);

    // Update averages
    this->updateFastAverageWithDiff(diff);
    this->updateAccurateAverageWithDiff(diff);
}

long Scale::GetOffset() {
    return this->loadCell.get_offset();
}

void Scale::updateFastAverageWithDiff(long diff) {
    float values[this->fastAverage.getCount()];

    // collect old values
    for (int i = 0; i < this->fastAverage.getCount(); i++) {
        values[i] = this->fastAverage.getElement(i);
    }

    // update average with old values + diff
    for (int i = 0; i < this->fastAverage.getCount(); i++) {
        this->fastAverage.add(values[i] + diff);
    }
}

void Scale::updateAccurateAverageWithDiff(long diff) {
    float values[this->accurateAverage.getCount()];

    // collect old values
    for (int i = 0; i < this->accurateAverage.getCount(); i++) {
        values[i] = this->accurateAverage.getElement(i);
    }

    // update average with old values + diff
    for (int i = 0; i < this->accurateAverage.getCount(); i++) {
        this->accurateAverage.add(values[i] + diff);
    }
}
