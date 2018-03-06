#include "ArduinoLog.h"
#include "StateMachine.h"
#include "Arduino.h"
#include "BottleType.h"

StateMachine::StateMachine():
fillRate(FILL_RATE_SAMPLES),
timeBetweenWeightUpdates(FILL_RATE_SAMPLES)
{
    this->CurrentState = StateMachine::State::Waiting;
    this->currentBottleType = UNKNOWN_BOTTLE;
}

StateMachine::StateMachine(Scale scale, Valve valve, PinButton greenButton, PinButton redButton): StateMachine::StateMachine()
{
    this->scale = scale;
    this->valve = valve;
    this->greenButton = greenButton;
    this->redButton = redButton;

    loadBottles(this->bottleTypes);
}

void StateMachine::Loop()
{
    /* int state = static_cast<int>(this->CurrentState); */
    /* Serial.print("state: "); */
    /* Serial.println(state); */

    switch (this->CurrentState) {
    case StateMachine::State::Waiting:
        this->WaitingLoop();
        break;
    case StateMachine::State::Filling:
        this->FillingLoop();
        break;
    case StateMachine::State::FillingPaused:
        this->FillingPausedLoop();
        break;
    case StateMachine::State::Filled:
        this->FilledLoop();
        break;
    case StateMachine::State::Calibrating:
        this->CalibratingLoop();
        break;
    case StateMachine::State::Menu:
        this->MenuLoop();
        break;
    default:
        Serial.println("Unknown state!");
        break;
    }
}

void StateMachine::WaitingLoop()
{
    ScaleUpdate update = this->scale.Update();
    if (update.WeightIsPlaced) {
        BottleType bottleType = getBottleBasedOnWeight(update.Weight, this->bottleTypes);

        if (bottleType != UNKNOWN_BOTTLE) {
            return this->ChangeStateFromWaitingToFilling(update, bottleType);
        }
    }

    /* bool smallUpdate = !updateAccurate.WeightIsRemoved && !updateAccurate.WeightIsPlaced; */
    bool newStable = update.WeightIsStable && !update.OldWeightIsStable;

    if (update.WeightIsRemoved && newStable) {
        this->scale.Tare();
    }
}

void StateMachine::ChangeStateFromWaitingToFilling(ScaleUpdate update, BottleType bottleType)
{
    Log.notice(F("Exiting waiting state"));

    this->currentBottleType = bottleType;
    this->currentBottleWeight = update.Weight;
    this->resetFillingStopWatch();
    this->resetLoopCounter();
    this->resetAverageFillRateAndTime();
    this->valve.Open();

    // print report as last because it takes some time
    this->printReport(update);

    this->CurrentState = StateMachine::State::Filling;
    Log.notice(F("Entering filling state"));
}

void StateMachine::FillingLoop()
{
    // Handle report
    this->loopCounter++;

    // Handle buttons
    this->updateButtons();

    // If either button is clicked: go to filled state
    if (this->greenButton.isClick() || this->redButton.isClick()) {
        return this->ChangeStateFromFillingToFillingPaused();
    }

    // Handle scale
    ScaleUpdate update = this->scale.Update();

    if (update.WeightIsRemoved) {
        Log.notice(F("Weight is removed before bottle is filled"));
        return this->ChangeStateFromFillingToWaiting();
    }

    /* // check if weight is updated */
    if (update.WeightUpdated) {
        // Update fillrate
        this->updateAverageFillRate(update);

        // calculate full weight (bottle weight + liquid)
        int fullWeight = this->getFullWeight();

        // @TODO: better check i n readings are bigger then required weight
        if (update.Weight > fullWeight) {
            return this->ChangeStateFromFillingToFilled(update);
        }

        // @TODO:
        // - calculate if fill weight is reached before next loop (samples per // second)
        // - if so: calculate the ms required to reach fill weight, delay by
        // that amount, close valve, update weight and change to filled state
        // - Or work percentage based? Filled 90% -> 95%, 95% -98%, 99% ->100%?
        int fillRate = this->fillRate.getAverage();
        if (fillRate + update.Weight >= fullWeight) {
            // bottle is filled before next update
            uint32_t timeToUpdate =  round(this->timeBetweenWeightUpdates.getMedian());
            int neededWeight = fullWeight - update.Weight;
            uint32_t timeToFull = round((neededWeight / fillRate) * timeToUpdate);

            // Ope valve in case timeToFull was too short and this statement is
            // executed a second time
            this->valve.Open();
            delay(timeToFull);
            this->valve.Close();
        }
    }
}

void StateMachine::ChangeStateFromFillingToFillingPaused()
{
    Log.notice(F("Exiting filling state"));

    // close valve
    this->valve.Close();

    // wait for all buttons to be released
    this->waitForButtonsToBeReleased();

    Log.notice(F("Entering filling paused state"));
    this->CurrentState = StateMachine::State::FillingPaused;
}

void StateMachine::FillingPausedLoop()
{
    // Handle buttons
    this->updateButtons();

    // if green button is clicked: return to filling state
    if (this->greenButton.isClick()) {
        return this->ChangeStateFromFillingPausedToFilling();
    }

    // Handle scale
    ScaleUpdate update = this->scale.Update();

    // check if weight is removed
    if (update.WeightIsRemoved) {
        return this->ChangeStateFromFillingPausedToWaiting();
    }
}

void StateMachine::ChangeStateFromFillingPausedToFilling()
{
    Log.notice(F("Exiting filling paused state"));

    // close valve
    this->valve.Open();

    Log.notice(F("Entering waiting state"));
    this->CurrentState = StateMachine::State::Waiting;
}

/**
 * Happens when bottle is removed while filling is paused
 */
void StateMachine::ChangeStateFromFillingPausedToWaiting()
{
    Log.notice(F("Exiting filling paused state"));

    // close valve
    this->valve.Close();

    // reset bottle
    this->resetBottle();

    // reset scale
    this->scale.Tare();

    Log.notice(F("Entering waiting state"));
    this->CurrentState = StateMachine::State::Waiting;
}

/**
 * Happens when bottle is removed mid filling
 */
void StateMachine::ChangeStateFromFillingToWaiting()
{
    Log.notice(F("Exiting filling state"));

    // close valve
    this->valve.Close();

    // reset bottle
    this->resetBottle();

    // reset scale
    this->scale.Tare();

    Log.notice(F("Entering waiting state"));
    this->CurrentState = StateMachine::State::Waiting;
}

void StateMachine::ChangeStateFromFillingToFilled(ScaleUpdate update)
{
    Log.notice(F("Exiting filling state"));

    // close valve
    this->valve.Close();

    // wait for all buttons to be released
    this->waitForButtonsToBeReleased();

    // print report as last because it takes some time
    this->printReport(update);

    Log.notice(F("Entering filled state"));
    this->CurrentState = StateMachine::State::Filled;
}

void StateMachine::FilledLoop()
{
    // Handle buttons
    this->updateButtons();

    // if green button is long down: open valve
    if (this->greenButton.isLongClick()) {
        return this->valve.Open();
    }

    // if green button is up: close valve
    if (this->greenButton.isReleased()) {
        return this->valve.Close();
    }

    // Handle scale
    ScaleUpdate update = this->scale.Update();

    // check if weight is removed
    if (update.WeightIsRemoved) {
        Log.notice(F("WeightDiff: %l"), update.WeightDiff);
        return this->ChangeStateFromFilledToWaiting();
    }
}

void StateMachine::ChangeStateFromFilledToWaiting()
{
    Log.notice(F("Exiting filled state"));

    // close valve
    this->valve.Close();

    // reset scale
    this->scale.Tare();

    Log.notice(F("Entering waiting state"));
    this->CurrentState = StateMachine::State::Waiting;
}

void StateMachine::CalibratingLoop()
{
    Serial.println("calibrating");
}

void StateMachine::MenuLoop()
{
    Serial.println("menu");
}

void StateMachine::updateButtons()
{
    this->greenButton.update();
    this->redButton.update();
}

int StateMachine::getFullWeight()
{
    return this->currentBottleWeight + this->currentBottleType.LiquidWeight;
}

void StateMachine::waitForButtonsToBeReleased()
{
    while(true) {
        this->updateButtons();
        if (this->greenButton.isIdle() && this->redButton.isIdle()) {
            break;
        }
    }
}

void StateMachine::resetBottle()
{
    this->currentBottleType = UNKNOWN_BOTTLE;
    this->currentBottleWeight = 0.0;
}

void StateMachine::resetFillingStopWatch()
{
    this->fillingStopWatch.reset();
    this->fillingStopWatch.start();
}

void StateMachine::resetLoopCounter()
{
    this->loopCounter = 0;
}

void StateMachine::printReport(ScaleUpdate update)
{
    // @TODO: print fill report:
    // - time it took
    // - bottle/start weight
    // - bottle type
    // - end weight
    // - weight over time?
    // - scale offset

    int fullWeightDeviation = update.Weight - this->getFullWeight();
    Log.notice(F("{"
        "\"time\": %l,"
        "\"bottle_weight\": %l, "
        "\"full_weight\": %l, "
        "\"full_weight_deviation\": %l, "
        "\"average_fill_rate\": %l, "
        "\"loops\": %d, "
        "\"bottle_type\": {"
            "\"name\": \"%s\", "
            "\"min_weight\": %l, "
            "\"max_weight\": %l, "
            "\"liquid_weight\": %l "
        "}, "
        "\"scale_update\": {"
            "\"old_weight\": %l, "
            "\"weight\": %l, "
            "\"old_stable_weight\": %l, "
            "\"stable_weight\": %l, "
            "\"weight_is_removed\": \"%t\", "
            "\"weight_is_placed\": \"%t\", "
            "\"weight_diff\": %l, "
            "\"old_weight_is_stable\": \"%t\", "
            "\"weight_is_stable\": \"%t\", "
            "\"stable_weight_updated\": \"%t\", "
            "\"average_weight_updated\": \"%t\" "
        "} "
    "}"),
            this->fillingStopWatch.elapsed(),
            this->currentBottleWeight,
            this->getFullWeight(),
            fullWeightDeviation,
            this->fillRate.getMedian(),
            this->loopCounter,

            this->currentBottleType.Name.c_str(),
            this->currentBottleType.MinWeight,
            this->currentBottleType.MaxWeight,
            this->currentBottleType.LiquidWeight,

            update.OldWeight,
            update.Weight,
            update.OldStableWeight,
            update.StableWeight,
            update.WeightIsRemoved,
            update.WeightIsPlaced,
            update.WeightDiff,
            update.OldWeightIsStable,
            update.WeightIsStable,
            update.StableWeightUpdated,
            update.WeightUpdated
            );
}

void StateMachine::updateAverageFillRateAndTime(ScaleUpdate update)
{
    this->updateAverageFillRate(update);
    this->updateAverageTimeBetweenWeightUpdates();
}

// @TODO: remove this?
// I can calculate average fill rate by using the fillingStopWatc, empty bottle
// weight and current weight...
void StateMachine::updateAverageFillRate(ScaleUpdate update)
{
    /* long weightDiff = update.Weight - update.OldWeight; */
    /* float fillRate = (this->fillRate * this->fillRateSamples) + weightDiff; */
    /* this->fillRateSamples++; */
    /* this->fillRate = round(fillRate / this->fillRateSamples); */
    this->fillRate.add(update.WeightDiff);
}

void StateMachine::updateAverageTimeBetweenWeightUpdates()
{
    uint8_t i = this->timeBetweenWeightUpdates.getSize();
    int timeDiff = this->fillingStopWatch.elapsed();
    if (i > 0) {
        // current value - last value
        int lastElement = round(this->timeBetweenWeightUpdates.getElement(i - i));
        timeDiff = this->fillingStopWatch.elapsed() - lastElement;
    }

    this->timeBetweenWeightUpdates.add(timeDiff);
}

void StateMachine::resetAverageFillRate()
{
    this->fillRate.clear();
}

void StateMachine::resetAverageFillRateAndTime()
{
    this->fillRate.clear();
    this->timeBetweenWeightUpdates.clear();
}
