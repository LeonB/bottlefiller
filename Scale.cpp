#include "Scale.h"

Scale::Scale() {
}

Scale::Scale(int pinDout, int pinSck) {
    this->loadCell.begin(pinDout, pinSck);
    this->chrono.start();
    this->measurementsPerSecond = DEFAULT_SCALE_MEASUREMENTS_PER_SECOND;
    this->runningAverage = RunningAverage(10);
}

void Scale::Update() {
    /* this->loadCell.read_average(20); */
    /* this->loadCell.tare(); // reset the scale to 0 */

    if (this->chrono.hasPassed(1000/this->measurementsPerSecond)) {
        this->runningAverage.addValue(this->loadCell.read());
        /* Serial.println("Resetting scale chrono"); */
        this->chrono.restart();
        /* Serial.println(this->loadCell.read()); */
        Serial.println(this->runningAverage.getFastAverage());
    }
}
