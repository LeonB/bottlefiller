#include "Scale.h"
#include "Valve.h"
#include "BottleType.h"
#include <PinButton.h>
#include <StopWatch.h>
#include <RunningMedian.h>

#ifndef STATE_MACHINE
#define STATE_MACHINE

const uint8_t FILL_RATE_SAMPLES = 9;

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
    void restartFillingStopWatch();
    void restartUpdateStopWatch();
    void resetLoopCounter();
    void printReport(ScaleUpdate);
    BottleType currentBottleType;
    long currentBottleWeight;
    Scale scale;
    Valve valve;
    PinButton greenButton;
    PinButton redButton;
    BottleType bottleTypes[MAX_BOTTLE_TYPES];
    StopWatch fillingStopWatch;
    StopWatch updateStopWatch;
    unsigned int loopCounter;

    long getAverageFillRate(ScaleUpdate);
    long getTimeBetweenWeightUpdates();
};

#endif
