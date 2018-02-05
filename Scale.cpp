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
    this->average = RunningMedian(DEFAULT_SCALE_MEASUREMENTS_PER_SECOND);

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
    uint8_t readingsToAdd = DEFAULT_SCALE_MEASUREMENTS_PER_SECOND - this->average.getCount();
    Serial.print("Taking ");
    Serial.print(readingsToAdd);
    Serial.println(" new readings");

    // take enough readings to get a proper median
    while (this->average.getCount() < this->average.getSize()) {
        this->average.add(this->loadCell.get_value());
    }

    Serial.println("Waiting for readings to stabilise");
    double value = this->GetAccurateValue();
    while (value > 100.0 || value < -100.0) {
        /* this->Update(); */
        if (this->chrono.hasPassed(1000/this->measurementsPerSecond)) {
            this->average.add(this->loadCell.read());
            this->loadCell.set_offset(this->GetAccurateValue());
            value = this->GetAccurateValue();
            this->chrono.restart();
        }
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
        double oldValue = this->GetFastValue();
        // add reading to average
        this->average.add(this->loadCell.get_value());

        // restart chrono
        this->chrono.restart();
        return oldValue != this->GetFastValue();
    }
    return false;
}

// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
double Scale::GetAccurateValue()
{
    /* return this->average.getAverage(2); */
    return this->average.getMedian();
}

double Scale::GetFastValue()
{
    uint8_t count = this->average.getCount();
    uint8_t use = count;
    if (use > 3) {
        use = 3;
    }

    RunningMedian fastAverage(3);
    for (int i = 0; i < use; ++i) {
        double val = this->average.getElement(count - i);
        fastAverage.add(val);
    }

    // generate new median
    return fastAverage.getMedian();
}
