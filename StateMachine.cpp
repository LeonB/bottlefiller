#include "ArduinoLog.h"
#include "StateMachine.h"
#include "Arduino.h"

StateMachine::StateMachine()
{
    this->CurrentState = StateMachine::State::Waiting;
}

StateMachine::StateMachine(Scale scale): StateMachine::StateMachine()
{
    this->scale = scale;
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

    if (update.StableWeightUpdated) {
        Log.notice(F("New fast equilibrium"));
        Log.notice(F("New stable weight: %D"), update.StableWeight);
        Log.notice(F("Old stable weight: %D"), update.OldStableWeight);
        Log.notice(F("WeightDiff: %D"), update.WeightDiff);

        if (update.WeightIsRemoved) {
            Log.notice(F("Weight is removed"));
        }

        if (update.WeightIsPlaced) {
            Log.notice(F("Weight is placed"));
            /* BottleType bottleType = getBottleBasedOnWeight(update.Weight, bottleTypes); */
            /* Log.notice(F("Bottle type: %s"), bottleType.Name.c_str()); */
        }

        Log.notice(F("-------------------------------"));
    }
}

void StateMachine::FillingLoop()
{
    Serial.println("filling");
}

void StateMachine::FilledLoop()
{
    Serial.println("filled");
}

void StateMachine::CalibratingLoop()
{
    Serial.println("calibrating");
}

void StateMachine::MenuLoop()
{
    Serial.println("menu");
}
