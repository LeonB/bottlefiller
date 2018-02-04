#include <Chrono.h> 
#include "HX711.h"
#include "RunningAverage.h"

// defaults
#ifndef DEFAULT_SCALE_MEASUREMENTS_PER_SECOND
#define unsigned short int DEFAULT_SCALE_MEASUREMENTS_PER_SECOND = 10;
#endif

class Scale {
    public:
        Scale();
        Scale(int, int);
        void Update();

    private:
        // chrone is used for keeping track when measurements have to be taken
        Chrono chrono; 
        // measurementsPerSecond is used for determining when updates should
        // hapen
        unsigned short int measurementsPerSecond;
        HX711 loadCell;
        RunningAverage runningAverage;
};
