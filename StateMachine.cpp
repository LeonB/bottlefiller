#include "ArduinoLog.h"
#include "StateMachine.h"
#include "Arduino.h"
#include "BottleType.h"

StateMachine::StateMachine()
{
    this->CurrentState = StateMachine::State::Waiting;
    this->currentBottleType = UNKNOWN_BOTTLE;
}

StateMachine::StateMachine(Scale scale, Valve valve, BottleType bottleTypes[], PinButton greenButton, PinButton redButton): StateMachine::StateMachine()
{
    this->scale = scale;
    this->valve = valve;
    this->greenButton = greenButton;
    this->redButton = redButton;
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

        Log.notice(F("Weight is placed"));
        Log.notice(F("New stable weight: %D"), update.StableWeight);
        Log.notice(F("Old stable weight: %D"), update.OldStableWeight);
        Log.notice(F("WeightDiff: %D"), update.WeightDiff);
        Log.notice(F("Bottle type: %s"), bottleType.Name.c_str());

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

    this->valve.Close();
    this->currentBottleType = bottleType;
    this->currentBottleWeight = update.Weight;
    this->resetFillReport();

    this->CurrentState = StateMachine::State::Filling;
    Log.notice(F("Entering filling state"));
}

void StateMachine::FillingLoop()
{
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

    // check if weight is updated
    if (update.AverageWeightUpdated) {
        // calculate full weight (bottle weight + liquid)
        double fullWeight = this->getFullWeight();

        // @TODO: better check i n readings are bigger then required weight
        if (update.Weight > fullWeight) {
            return this->ChangeStateFromFillingToFilled();
        }

        // weight is still smaller then full weight
        if (update.Weight < fullWeight) {
            this->valve.Open();
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

void StateMachine::ChangeStateFromFillingToFilled()
{
    Log.notice(F("Exiting filling state"));

    // @TODO: print fill report:
    // - time it took
    // - bottle/start weight
    // - bottle type
    // - end weight
    // - weight over time?
    // - scale offset
    this->fillReport;
    Log.notice(F(R"END({
        "time": 0.0,
        "bottle_weight": 0.0,
        "bottle_type": {
        },
        "fill_weight": 0.0,
        "scale_offset": 0.0
    })END"));

    // close valve
    this->valve.Close();

    // wait for all buttons to be released
    this->waitForButtonsToBeReleased();

    Log.notice(F("Entering filled state"));
    this->CurrentState = StateMachine::State::Filled;
}

void StateMachine::FilledLoop()
{
    // Handle buttons
    this->updateButtons();

    // if green button is clicked: return to filling state
    if (this->greenButton.isSingleClick()) {
        return this->ChangeStateFromFilledToFilling();
    }

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
        Log.notice(F("WeightDiff: %D"), update.WeightDiff);
        return this->ChangeStateFromFilledToWaiting();
    }
}

void StateMachine::ChangeStateFromFilledToFilling()
{
    Log.notice(F("Exiting filled state"));

    // close valve
    this->valve.Close();

    Log.notice(F("Entering filling state"));
    this->CurrentState = StateMachine::State::Filling;
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

double StateMachine::getFullWeight()
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

void StateMachine::resetFillReport()
{
    this->fillReport.Reset();
    this->fillReport.bottleWeight = this->currentBottleWeight;
    this->fillReport.bottleType = this->currentBottleType;
    this->fillReport.scaleOffset = this->scale.GetOffset();
}
