#include "ArduinoLog.h"
#include "StateMachine.h"
#include "Arduino.h"
#include "BottleType.h"

const char msg_entering[] PROGMEM = "Entering %s state";
const char msg_exiting[] PROGMEM = "Exiting %s state";
const char msg_waiting[] PROGMEM = "waiting";
const char msg_filling[] PROGMEM = "filling";
const char msg_filling_paused[] PROGMEM = "filling paused";
const char msg_filled[] PROGMEM = "filled";

StateMachine::StateMachine()
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
        Log.error(F("Unknown state"));
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
    Log.notice(msg_exiting, msg_waiting);

    this->currentBottleType = bottleType;
    this->currentBottleWeight = update.Weight;
    this->restartFillingStopWatch();
    this->resetLoopCounter();
    this->valve.Open();

    // print report as last because it takes some time
    this->printReport(update);

    this->CurrentState = StateMachine::State::Filling;
    Log.notice(msg_entering, msg_filling);
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
        // calculate full weight (bottle weight + liquid)
        long fullWeight = this->getFullWeight();

        // @TODO: better check i n readings are bigger then required weight
        if (update.Weight > fullWeight) {
            return this->ChangeStateFromFillingToFilled(update);
        }

        // @TODO:
        // - calculate if fill weight is reached before next loop (samples per // second)
        // - if so: calculate the ms required to reach fill weight, delay by
        // that amount, close valve, update weight and change to filled state
        // - Or work percentage based? Filled 90% -> 95%, 95% -98%, 99% ->100%?
        long fillRate = this->getAverageFillRate(update);
        if (fillRate > 0 && fillRate + 400 + update.Weight >= fullWeight) {
            // bottle is filled before next update
            Log.notice("fillRate: %l", fillRate);

            unsigned long timeToUpdate =  this->getTimeBetweenWeightUpdates();
            Log.notice("timetoUpdate: %d", timeToUpdate);

            long neededWeight = fullWeight - update.Weight;
            Log.notice("neededWeight: %l", neededWeight);

            if (neededWeight > 400) {
                neededWeight = neededWeight - 400;
            } else {
                neededWeight = 0;
            }

            float percentage = ((float)neededWeight / (float)fillRate);
            Log.notice("percentage: %F", percentage);

            unsigned long timeToFull = round(percentage * timeToUpdate);
            Log.notice("timeToFull: %l", timeToFull);

            if (timeToFull > 20) {
                timeToFull = timeToFull - 20;
            } else {
                timeToFull = 0;
            }

            // Ope valve in case timeToFull was too short and this statement is
            // executed a second time
            this->valve.Open();
            delay(timeToFull);
            this->valve.Close();
            Serial.println("closed");
        }
    }
}

void StateMachine::ChangeStateFromFillingToFillingPaused()
{
    Log.notice(msg_exiting, msg_filling);

    // close valve
    this->valve.Close();

    // wait for all buttons to be released
    this->waitForButtonsToBeReleased();

    Log.notice(msg_entering, msg_filling_paused);
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
    Log.notice(msg_exiting, msg_filling_paused);

    // close valve
    this->valve.Open();

    Log.notice(msg_entering, msg_waiting);
    this->CurrentState = StateMachine::State::Waiting;
}

/**
 * Happens when bottle is removed while filling is paused
 */
void StateMachine::ChangeStateFromFillingPausedToWaiting()
{
    Log.notice(msg_exiting, msg_filling_paused);

    // close valve
    this->valve.Close();

    // reset bottle
    this->resetBottle();

    // reset scale
    this->scale.Tare();

    Log.notice(msg_entering, msg_waiting);
    this->CurrentState = StateMachine::State::Waiting;
}

/**
 * Happens when bottle is removed mid filling
 */
void StateMachine::ChangeStateFromFillingToWaiting()
{
    Log.notice(msg_exiting, msg_filling);

    // close valve
    this->valve.Close();

    // reset bottle
    this->resetBottle();

    // reset scale
    this->scale.Tare();

    Log.notice(msg_entering, msg_waiting);
    this->CurrentState = StateMachine::State::Waiting;
}

void StateMachine::ChangeStateFromFillingToFilled(ScaleUpdate update)
{
    Log.notice(msg_exiting, msg_filling);

    // close valve
    this->valve.Close();

    // wait for all buttons to be released
    this->waitForButtonsToBeReleased();

    // print report as last because it takes some time
    this->printReport(update);

    Log.notice(msg_entering, msg_filled);
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
        return this->ChangeStateFromFilledToWaiting();
    }
}

void StateMachine::ChangeStateFromFilledToWaiting()
{
    Log.notice(msg_exiting, msg_filled);

    // close valve
    this->valve.Close();

    // reset scale
    this->scale.Tare();

    Log.notice(msg_entering, msg_waiting);
    this->CurrentState = StateMachine::State::Waiting;
}

void StateMachine::CalibratingLoop()
{
    Log.notice(F("calibrating"));
}

void StateMachine::MenuLoop()
{
    Log.notice(F("menu"));
}

void StateMachine::updateButtons()
{
    this->greenButton.update();
    this->redButton.update();
}

long StateMachine::getFullWeight()
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

void StateMachine::restartFillingStopWatch()
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

    long fullWeightDeviation = update.Weight - this->getFullWeight();
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
            this->getAverageFillRate(update),
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

long StateMachine::getAverageFillRate(ScaleUpdate update)
{
    long weightDiff = update.Weight - this->currentBottleWeight;
    long elapsed = this->fillingStopWatch.elapsed();
    return round(weightDiff/elapsed);
}

long StateMachine::getTimeBetweenWeightUpdates()
{
    return this->scale.GetTimeBetweenWeightUpdates();
}
