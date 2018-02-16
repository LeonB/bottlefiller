#include "ArduinoLog.h"
#include "StateMachine.h"
#include "Arduino.h"
#include "BottleType.h"

StateMachine::StateMachine()
{
    this->CurrentState = StateMachine::State::Waiting;
    this->currentBottleType = UNKNOWN_BOTTLE;
}

StateMachine::StateMachine(Scale scale, Valve valve, BottleType bottleTypes[]): StateMachine::StateMachine()
{
    this->scale = scale;
    this->valve = valve;
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
        Log.notice(F("Weight is placed"));
        Log.notice(F("New stable weight: %D"), update.StableWeight);
        Log.notice(F("Old stable weight: %D"), update.OldStableWeight);
        Log.notice(F("WeightDiff: %D"), update.WeightDiff);
        BottleType bottleType = getBottleBasedOnWeight(update.Weight, this->bottleTypes);
        Log.notice(F("Bottle type: %s"), bottleType.Name.c_str());
        this->ChangeStateFromWaitingToFilling(bottleType);
    }

    /* bool smallUpdate = !updateAccurate.WeightIsRemoved && !updateAccurate.WeightIsPlaced; */
    bool newStable = update.WeightIsStable && !update.OldWeightIsStable;

    if (update.WeightIsRemoved && newStable) {
        // This tare updates the average and offset, but then the update
        // doesn't get reset...
        this->scale.Tare();
        // So reset update again
        /* this->scale.updateStatusAccurate(update); */
    }
}

void StateMachine::ChangeStateFromWaitingToFilling(BottleType bottleType)
{
    Log.notice(F("Exiting waiting state"));
    this->CurrentState = StateMachine::State::Filling;
    this->currentBottleType = bottleType;
    Log.notice(F("Entering filling state"));
}

void StateMachine::FillingLoop()
{
    ScaleUpdate update = this->scale.Update();
    if (update.WeightIsRemoved) {
        Log.notice(F("Weight is removed before bottle is filled"));
        this->ChangeStateFromFillingToWaiting();
    }

    if (update.AverageWeightUpdated) {
        // @TODO: better check i n readings are bigger then required weight
        double maxWeight = this->currentBottleWeight + this->currentBottleType.LiquidWeight;
        if (update.Weight > maxWeight) {
            this->ChangeStateFromFillingToFilled();
        }
    }
}

void StateMachine::ChangeStateFromFillingToWaiting()
{
    Log.notice(F("Exiting filling state"));
    this->currentBottleType = UNKNOWN_BOTTLE;
    this->CurrentState = StateMachine::State::Waiting;
    Log.notice(F("Entering waiting state"));
}

void StateMachine::ChangeStateFromFillingToFilled()
{
    Log.notice(F("Exiting filling state"));
    this->CurrentState = StateMachine::State::Filled;
    Log.notice(F("Entering filled state"));
}

void StateMachine::FilledLoop()
{
    ScaleUpdate update = this->scale.Update();
    if (update.WeightIsRemoved) {
        Log.notice(F("WeightDiff: %D"), update.WeightDiff);
        this->ChangeStateFromFilledToWaiting();
    }
}

void StateMachine::ChangeStateFromFilledToWaiting()
{
    Log.notice(F("Exiting filled state"));
    this->CurrentState = StateMachine::State::Waiting;
    Log.notice(F("Entering waiting state"));
}

void StateMachine::CalibratingLoop()
{
    Serial.println("calibrating");
}

void StateMachine::MenuLoop()
{
    Serial.println("menu");
}
