#include "Scale.h"
#include "Valve.h"
#include "BottleType.h"

#ifndef STATE_MACHINE
#define STATE_MACHINE

/**
 * States:
 *   - waiting for bottle / empty
 *   - filling
 *     - with timeout if it takes to long?
 *   - waiting for bottle to be removed / filled
 *   - tare?
 *   - menu
*/

class StateMachine
{
public:
    StateMachine();
    StateMachine(Scale, Valve, BottleType[]);
    void Loop();
    void WaitingLoop();
    void ChangeStateFromWaitingToFilling(BottleType);
    void FillingLoop();
    void ChangeStateFromFillingToWaiting();
    void ChangeStateFromFillingToFilled();
    void FilledLoop();
    void ChangeStateFromFilledToWaiting();
    void CalibratingLoop();
    void MenuLoop();
    enum class State {
        Waiting,
        Filling,
        Filled,
        Calibrating,
        Menu,
    };
    State CurrentState;
private:
    BottleType currentBottleType;
    double currentBottleWeight;
    Scale scale;
    Valve valve;
    BottleType bottleTypes[];
};

#endif
