#include "StateMachine.h"
#include "Arduino.h"

StateMachine::StateMachine()
{
    this->CurrentState = StateMachine::State::Waiting;
}

void StateMachine::Loop()
{
    switch (this->CurrentState) {
    case StateMachine::State::Waiting:
        this->WaitingLoop();
        break;
    case StateMachine::State::Filling:
        this->FillingLoop();
        break;
    case StateMachine::State::Filled:
        this->WaitingLoop();
        break;
    case StateMachine::State::Calibrating:
        this->WaitingLoop();
        break;
    case StateMachine::State::Menu:
        this->MenuLoop();
        break;
    }
}

void StateMachine::WaitingLoop()
{
}

void StateMachine::FillingLoop()
{
}

void StateMachine::FilledLoop()
{
}

void StateMachine::CalibratingLoop()
{
}

void StateMachine::MenuLoop()
{
}
