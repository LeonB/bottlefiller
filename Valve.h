#ifndef VALVE
#define VALVE

/**
 * States:
 *   - waiting for bottle / empty
 *   - filling
 *     - with timeout if it takes to long?
 *   - waiting for bottle to be removed / filled
 *   - tare?
 *   - menu
*/

class Valve
{
public:
    Valve();
    Valve(int);
    void Open();
    void Close();
private:
    int pin;
};

#endif
