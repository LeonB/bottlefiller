#include "Scale.h"
#include "ArduinoLog.h"

Scale::Scale()
{
}

Scale::Scale(int pinDout, int pinSck)
{
    this->loadCell.begin(pinDout, pinSck);
    this->loadCell.set_offset(0.0);
    this->chrono.start();

    // initialize the running median correctly
    this->average = RunningMedian(this->measurementsPerSecond);

    Log.notice(F("Waiting for load cell to become ready"));
    while (!this->loadCell.is_ready()) {
        ;
    }

    // setup zero weight scale
    this->Tare();
}

void Scale::Tare()
{
    Log.notice(F("Tare"));

    // take enough readings to get a proper median
    struct Update update = this->Update();
    if (this->average.getCount() < this->average.getSize()) {
        Log.notice(F("Filling average with samples"));
        while (this->average.getCount() < this->average.getSize()) {
            update = this->Update();
        }
    }

    Log.notice(F("Waiting for readings to stabilise"));
    while (!update.WeightIsStable) {
        update = this->Update();
    }

    // readings have stabilised: set new offset
    Log.notice(F("Readings have stabilised at %D"), update.StableWeight);
    this->SetOffset(update.StableWeight);

    // set initial median as default offset
    Log.notice(F("Initial load cell offset: %l"), this->GetOffset());
}

struct Update Scale::Update()
{
    struct Update updateFast = {
        this->weightFast, // OldWeight
        this->weightFast, // Weight
        this->stableWeightFast, // OldStableWeight
        this->stableWeightFast, // StableWeight
        false, // WeightIsRemoved
        false, // WeightIsPlaced
        0.0, // WeightDiff
        this->calculateIfWeightIsStableFast(), // OldWeightIsStable
        this->calculateIfWeightIsStableFast(), // WeightIsStable
        false, // StableWeightUpdated
        false, // AverageUpdated
    };

    struct Update updateAccurate = {
        this->weightAccurate, // OldWeight
        this->weightAccurate, // Weight
        this->stableWeightAccurate, // OldStableWeight
        this->stableWeightAccurate, // StableWeight
        false, // WeightIsRemoved
        false, // WeightIsPlaced
        0.0, // WeightDiff
        this->calculateIfWeightIsStableAccurate(), // OldWeightIsStable
        this->calculateIfWeightIsStableAccurate(), // WeightIsStable
        false, // StableWeightUpdated
        false, // AverageUpdated
    };

    // test if at least 100ms (in the default case) have passed
    if (this->chrono.hasPassed(1000/this->measurementsPerSecond)) {
        // get new value
        double value = this->loadCell.get_value();

        // add reading to average
        this->average.add(value);
        /* this->fastAverage.add(value); */

        // restart chrono
        this->chrono.restart();

        updateFast = this->updateStatusFast(updateFast);
        updateAccurate = this->updateStatusAccurate(updateAccurate);

        /* bool smallUpdate = !updateAccurate.WeightIsRemoved && !updateAccurate.WeightIsPlaced; */
        bool newStableAccurate = updateAccurate.WeightIsStable && !updateAccurate.OldWeightIsStable;

        if (updateAccurate.WeightIsRemoved && newStableAccurate) {
            // This tare updates the average and offset, but then the update
            // doesn't get reset...
            this->Tare();
            // So reset update again
            updateAccurate = this->updateStatusAccurate(updateAccurate);
        }

        return updateFast;
        /* return updateAccurate; */
    }

    return updateFast;
}

struct Update Scale::updateStatusFast(struct Update update)
{
    // get new weight
    update.Weight = this->calculateWeightFast();

    // calculate if weight is stable
    update.WeightIsStable = this->calculateIfWeightIsStableFast();

    // update scale update with new information
    struct Update updateFast = this->updateStatus(update);

    // Update scale object with new weights
    // Should this be done here?
    this->weightFast = updateFast.Weight;
    this->stableWeightFast = updateFast.StableWeight;

    return updateFast;
}

struct Update Scale::updateStatusAccurate(struct Update update)
{
    // get new weight
    update.Weight = this->calculateWeightAccurate();

    // calculate if weight is stable
    update.WeightIsStable = this->calculateIfWeightIsStableAccurate();

    // update scale update with new information
    struct Update updateAccurate = this->updateStatus(update);

    // Update scale object with new weights
    // Should this be done here?
    this->weightAccurate = updateAccurate.Weight;
    this->stableWeightAccurate = updateAccurate.StableWeight;

    return updateAccurate;
}

struct Update Scale::updateStatus(struct Update update)
{
    // Keep this clean of this->... calls

    if (update.Weight != update.OldWeight) {
        update.AverageWeightUpdated = true;
    }

    if (update.OldWeightIsStable == false && update.WeightIsStable == true) {
        update.WeightDiff = update.OldStableWeight - update.Weight;

        if (update.WeightDiff > this->maxWeightDiffToBeStable) {
            update.WeightIsRemoved = true;
            update.WeightIsPlaced = false;
        } else if (update.WeightDiff < -this->maxWeightDiffToBeStable) {
            update.WeightIsRemoved = false;
            update.WeightIsPlaced = true;
        } else {
            update.WeightIsRemoved = false;
            update.WeightIsPlaced = false;
        }

        // In both cases: there is a new equilibrium and there's a new
        // stable weight
        update.StableWeight = update.Weight;
        update.StableWeightUpdated = true;

        return update;
    }

    update.WeightIsStable = false;
    update.WeightIsPlaced = false;
    update.WeightIsRemoved = false;
    return update;
}

RunningMedian Scale::fastAverage()
{
    uint8_t needed = 3;
    uint8_t count = this->average.getCount();
    uint8_t use = count;
    if (use > needed) {
        use = needed;
    }

    RunningMedian fastAverage(needed);
    for (int i = 0; i < use; ++i) {
        double val = this->average.getElement(count -1 - i);
        fastAverage.add(val);
    }

    return fastAverage;
}

// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
double Scale::calculateWeightFast()
{
    return this->fastAverage().getMedian();
}

// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
double Scale::calculateWeightAccurate()
{
    /* return this->average.getAverage(2); */
    return this->average.getMedian();
}

bool Scale::calculateIfWeightIsStableFast()
{
    RunningMedian fastAverage = this->fastAverage();
    double diff = fastAverage.getHighest() - fastAverage.getLowest();
    return diff < 250;
}

bool Scale::calculateIfWeightIsStableAccurate()
{
    double diff = this->average.getHighest() - this->average.getLowest();
    return diff < 750;
}

void Scale::SetOffset(long stableWeight) {
    // oldOffset = -100
    // stableWeight = 20
    // diff = 20
    // newOffset = -80
    //
    // oldOffset = 80
    // stableWeight = 0
    // diff = 0
    // newOffset = 80

    // oldOffset = 80
    // stableWeight = 60
    // diff = -60
    // newOffset = 140
    this->UpdateOffset(stableWeight);
}

void Scale::UpdateOffset(long diff) {
    long oldOffset = this->GetOffset();
    long newOffset = oldOffset + diff;
    this->loadCell.set_offset(newOffset);

    // Update averages
    /* this->updateFastAverageWithDiff(diff); */
    this->updateAccurateAverageWithDiff(diff);

    // Update values
    this->weightAccurate -= diff;
    this->stableWeightAccurate -= diff;
    this->weightFast -= diff;
    this->stableWeightFast -= diff;
}

long Scale::GetOffset() {
    return this->loadCell.get_offset();
}

RunningMedian Scale::updateAccurateAverageWithDiff(long diff) {
    this->average = this->updateAverageWithDiff(this->average, diff);
    return this->average;
}

RunningMedian Scale::updateAverageWithDiff(RunningMedian average, long diff) {
    float values[average.getCount()];

    // collect old values
    for (int i = 0; i < average.getCount(); i++) {
        values[i] = average.getElement(i);
    }

    // update average with old values + diff
    for (int i = 0; i < average.getCount(); i++) {
        average.add(values[i] - diff);
    }

    return average;
}

void Scale::SetMeasurementsPerSecond(int measurements)
{
    int oldCount = this->average.getCount();

    if (oldCount == measurements) {
        return;
    }

    // collect old values
    float values[oldCount];
    for (int i = 0; i < oldCount; i++) {
        values[i] = this->average.getElement(i);
    }

    // initialize new average
    this->measurementsPerSecond = measurements;
    this->average = RunningMedian(this->measurementsPerSecond);

    // update new average with old values
    for (int i = 0; i < oldCount; i++) {
        this->average.add(values[i]);
    }
}

void Scale::SetMaxWeightDiffToBeStable(unsigned int maxWeight)
{
    this->maxWeightDiffToBeStable = maxWeight;
}
