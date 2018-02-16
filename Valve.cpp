#include "Valve.h"
#include "Arduino.h"

Valve::Valve() {}

Valve::Valve(int pin)
{
    this->pin = pin;
    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, LOW);
}

void Valve::Open()
{
    digitalWrite(this->pin, HIGH);
}

void Valve::Close()
{
    digitalWrite(this->pin, LOW);
}
