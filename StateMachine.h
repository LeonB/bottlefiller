#include "Scale.h"
#include "Valve.h"
#include "BottleType.h"
#include "PinButton.h"

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
    StateMachine(Scale, Valve, BottleType[], PinButton, PinButton);
    void Loop();
    void WaitingLoop();
    void ChangeStateFromWaitingToFilling(ScaleUpdate, BottleType);
    void FillingLoop();
    void ChangeStateFromFillingToWaiting();
    void ChangeStateFromFillingToFilled();
    void FilledLoop();
    void ChangeStateFromFilledToFilling();
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
    void updateButtons();
    double getFullWeight();
    void waitForButtonsToBeReleased();
    void resetBottle();
    BottleType currentBottleType;
    double currentBottleWeight;
    Scale scale;
    Valve valve;
    PinButton greenButton;
    PinButton redButton;
    BottleType bottleTypes[];
};

#endif
