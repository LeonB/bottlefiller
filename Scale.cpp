#include "Scale.h"

Scale::Scale()
{
}

Scale::Scale(int pinDout, int pinSck)
{
    this->loadCell.begin(pinDout, pinSck);
    this->loadCell.tare(DEFAULT_SCALE_MEASUREMENTS_PER_SECOND*10);
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

    // take enough readings to get a proper median
    while (this->accurateAverage.getCount() < this->accurateAverage.getSize()) {
        double value = this->loadCell.get_value();
        this->accurateAverage.add(value);
        this->fastAverage.add(value);
    }

    Serial.println("Waiting for readings to stabilise");
    /* double value = this->GetAccurateValue(); */
    while (!this->GetWeightIsStableAccurate()) {
        this->Update();
        this->loadCell.set_offset(this->GetAccurateValue());
        /* if (this->chrono.hasPassed(1000/this->measurementsPerSecond)) { */
        /*     this->average.add(this->loadCell.read()); */
        /*     this->loadCell.set_offset(this->GetAccurateValue()); */
        /*     value = this->GetAccurateValue(); */
        /*     this->chrono.restart(); */
        /* } */
    }

    Serial.print("Readings have stabilised");

    // set initial median as default offset
    Serial.print("Initial offset: ");
    Serial.println(this->loadCell.get_offset());
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

        // get new value
        double value = this->loadCell.get_value();

        // add reading to average
        this->accurateAverage.add(value);
        this->fastAverage.add(value);

        // restart chrono
        this->chrono.restart();

        // calculate if weight has updated
        bool fastUpdated = oldFastValue != this->GetFastValue();
        bool accurateUpdated = oldAccurateValue != this->GetAccurateValue();

        // calculate if weight is stable
        this->weightIsStableFast = this->calculateIfWeightIsStableFast();
        this->weightIsStableAccurate = this->calculateIfWeightIsStableAccurate();

        if (oldStableFast == false && this->weightIsStableFast == true && !this->newFastEquilibrium) {
            this->newFastEquilibrium= true;
        } else {
            this->newFastEquilibrium = false;
        }

        if (oldStableAccurate == false && this->weightIsStableAccurate == true && !this->newAccurateEquilibrium) {
            this->newAccurateEquilibrium = true;
        } else {
            this->newAccurateEquilibrium = false;
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
