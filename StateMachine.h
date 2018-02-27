#include "Scale.h"
#include "Valve.h"
#include "BottleType.h"
#include "PinButton.h"
#include "StopWatch.h"

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
    StateMachine(Scale, Valve, PinButton, PinButton);
    void Loop();
    void WaitingLoop();
    void ChangeStateFromWaitingToFilling(ScaleUpdate, BottleType);
    void FillingLoop();
    void ChangeStateFromFillingToWaiting();
    void ChangeStateFromFillingToFillingPaused();
    void FillingPausedLoop();
    void ChangeStateFromFillingPausedToWaiting();
    void ChangeStateFromFillingPausedToFilling();
    void ChangeStateFromFillingToFilled(ScaleUpdate);
    void FilledLoop();
    void ChangeStateFromFilledToWaiting();
    void CalibratingLoop();
    void MenuLoop();
    enum class State {
        Waiting,
        Filling,
        FillingPaused,
        Filled,
        Calibrating,
        Menu,
    };
    State CurrentState;
private:
    void updateButtons();
    long getFullWeight();
    void waitForButtonsToBeReleased();
    void resetBottle();
    void resetFillingStopWatch();
    void printReport(ScaleUpdate);
    BottleType currentBottleType;
    long currentBottleWeight;
    Scale scale;
    Valve valve;
    PinButton greenButton;
    PinButton redButton;
    BottleType bottleTypes[MAX_BOTTLE_TYPES];
    StopWatch fillingStopWatch;
};

#endif
