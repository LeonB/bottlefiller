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
    /* this->fastAverage = RunningMedian(3); */
    this->average = RunningMedian(DEFAULT_SCALE_MEASUREMENTS_PER_SECOND);
    /* this->scaleIsEmpty = true; */
    this->stableWeightFast = 0.0;
    this->stableWeightAccurate = 0.0;

    Serial.println(F("Waiting for load cell to become ready"));
    while (!this->loadCell.is_ready()) {
        ;
    }

    // setup zero weight scale
    this->Tare();
}

void Scale::Tare()
{
    // calculate how many new reading to take to fill average buffer
    uint8_t readingsToAdd = DEFAULT_SCALE_MEASUREMENTS_PER_SECOND - this->average.getCount();
    Serial.print("Taking ");
    Serial.print(readingsToAdd);
    Serial.println(" new readings");

    // reset average
    /* this->fastAverage.clear(); */
    /* this->average.clear(); */

    // take enough readings to get a proper median
    Serial.println("Filling average with samples");
    struct Update update = this->Update();
    while (this->average.getCount() < this->average.getSize()) {
        update = this->Update();
        if (update.AverageWeightUpdated) {
            Serial.println("New reading");
        }
    }

    Serial.println("Waiting for readings to stabilise");
    while (!update.WeightIsStable) {
        update = this->Update();
        if (update.AverageWeightUpdated) {
            Serial.println("New reading");
        }
    }

    // readings have stabilised: set new offset
    Serial.println("Readings have stabilised");
    this->SetOffset(update.StableWeight);

    // set initial median as default offset
    Serial.print("Initial load cell offset: ");
    Serial.println(this->GetOffset());
}

struct Update Scale::Update()
{
    struct Update update = {
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

        /* struct Update fastUpdated = this->updateFast(update); */
        struct Update accurateUpdate = this->updateAccurate(update);

        if (!accurateUpdate.StableWeightUpdated && accurateUpdate.AverageWeightUpdated) {
            Serial.println("tare()");
            Serial.println(accurateUpdate.OldWeight);
            Serial.println(accurateUpdate.Weight);
            Serial.println("--------------");
            // @TODO: inifinite loop
            /* this->Tare(); */
        }

        this->weightAccurate = accurateUpdate.Weight;
        this->stableWeightAccurate = accurateUpdate.StableWeight;

        return accurateUpdate;
        /* return fastUpdated.AverageUpdated || accurateUpdated.AverageUpdated; */
    }

    return update;
}

/* struct Update Scale::updateFast(struct Update update) */
/* { */
/*     // get old values */
/*     bool oldStableFast = this->weightIsStableFast; */
/*     double oldStableWeightFast = this->stableWeightFast; */

/*     // get new weight */
/*     double newFastWeight = this->GetWeightFast(); */

/*     // calculate if weight is stable */
/*     this->weightIsStableFast = this->calculateIfWeightIsStableFast(); */

/*     if (oldStableFast == false && this->weightIsStableFast == true && !this->newStableWeightFast) { */
/*         double diff = oldStableWeightFast - newFastWeight; */
/*         if (diff > 100.0) { */
/*             this->weightIsRemovedFast = true; */
/*             this->weightIsPlacedFast = false; */
/*             this->stableWeightFast = newFastWeight; */
/*         } else if (diff < -100.0) { */
/*             this->weightIsRemovedFast = false; */
/*             this->weightIsPlacedFast = true; */
/*         } else { */
/*             this->weightIsRemovedFast = false; */
/*             this->weightIsPlacedFast = false; */
/*         } */

/*         // In both cases: there is a new equilibrium and there's a new */
/*         // stable weight */
/*         this->newStableWeightFast = true; */
/*         this->stableWeightFast = newFastWeight; */

/*         return update; */
/*     } */

/*     update.StableWeight = false; */
/*     update.WeightIsPlaced = false; */
/*     update.WeightIsRemoved = false; */
/*     return update; */
/* } */

struct Update Scale::updateAccurate(struct Update update)
{
    // get new weight
    update.Weight = this->calculateWeightAccurate();

    // calculate if weight is stable
    update.WeightIsStable = this->calculateIfWeightIsStableAccurate();

    if (update.Weight != update.OldWeight) {
        update.AverageWeightUpdated = true;
    }

    if (update.OldWeightIsStable == false && update.WeightIsStable == true) {
        update.WeightDiff = update.OldStableWeight - update.Weight;

        Serial.print("update.WeightDiff: ");
        Serial.println(update.WeightDiff);

        if (update.WeightDiff > 100.0) {
            update.WeightIsRemoved = true;
            update.WeightIsPlaced = false;
        } else if (update.WeightDiff < -100.0) {
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

/* bool Scale::GetWeightIsStableFast() */
/* { */
/*     return this->weightIsStableFast; */
/* } */

/* bool Scale::GetWeightIsStableAccurate() */
/* { */
/*     return this->weightIsStableAccurate; */
/* } */

/* bool Scale::NewStableWeightFast() */
/* { */
/*     return this->newStableWeightFast; */
/* } */

/* bool Scale::NewStableWeightAccurate() */
/* { */
/*     return this->newStableWeightAccurate; */
/* } */

/* bool Scale::WeightIsRemovedFast() */
/* { */
/*     return this->weightIsRemovedFast; */
/* } */

/* bool Scale::WeightIsRemovedAccurate() */
/* { */
/*     return this->weightIsRemovedAccurate; */
/* } */

/* bool Scale::WeightIsPlacedFast() */
/* { */
/*     return this->weightIsPlacedFast; */
/* } */

/* bool Scale::WeightIsPlacedAccurate() */
/* { */
/*     return this->weightIsPlacedAccurate; */
/* } */

/* double Scale::GetStableWeightFast() */
/* { */
/*     return this->stableWeightFast; */
/* } */

/* double Scale::GetStableWeightAccurate() */
/* { */
/*     return this->stableWeightAccurate; */
/* } */

void Scale::SetOffset(long offset) {
    long diff = offset - this->GetOffset();
    this->UpdateOffset(diff);
}

void Scale::UpdateOffset(long diff) {
    Serial.print("UpdateOffset: ");
    Serial.println(diff);

    long offset = this->GetOffset() + diff;
    this->loadCell.set_offset(offset);

    // Update averages
    /* this->updateFastAverageWithDiff(diff); */
    this->updateAccurateAverageWithDiff(diff);

    // Update values
    this->stableWeightFast -= diff;
    this->stableWeightAccurate -= diff;
}

long Scale::GetOffset() {
    return this->loadCell.get_offset();
}

/* void Scale::updateFastAverageWithDiff(long diff) { */
/*     float values[this->fastAverage.getCount()]; */

/*     // collect old values */
/*     for (int i = 0; i < this->fastAverage.getCount(); i++) { */
/*         values[i] = this->fastAverage.getElement(i); */
/*     } */

/*     // update average with old values + diff */
/*     for (int i = 0; i < this->fastAverage.getCount(); i++) { */
/*         this->fastAverage.add(values[i] + diff); */
/*     } */
/*     return; */
/* } */

void Scale::updateAccurateAverageWithDiff(long diff) {
    float values[this->average.getCount()];

    // collect old values
    for (int i = 0; i < 1; i++) {
        values[i] = this->average.getElement(i);
    }

    // update average with old values + diff
    for (int i = 0; i < this->average.getCount(); i++) {
        this->average.add(values[i] + diff);
    }
    return;
}

