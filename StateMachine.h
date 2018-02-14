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
    void Loop();
    void WaitingLoop();
    void FillingLoop();
    void FilledLoop();
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
};
