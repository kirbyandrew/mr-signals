/*
 * pin_sensor.cpp
 *
 *  Created on: Nov 18, 2018
 *      Author: ackpu
 */


#include "pin_sensor.h"

#ifndef ARDUINO
#include "arduino_mock.h"   // pinMode(), digitalWrite() for unit tests not on Arduino
#else
#include "Arduino.h"
#endif

using namespace mr_signals;

/// Initialize the pin into INPUT_PULLUP
Pin_sensor::Pin_sensor(uint8_t pin) : pin_(pin)
{
    pinMode(pin_,INPUT_PULLUP);
}

/// Directly read the pin each time this is called
bool Pin_sensor::is_active()
{
    if(HIGH == digitalRead(pin_)) {
        return true;
    }
    else {
        return false;
    }
}

/// Can always return false as the value of the pin is read directly in the
/// is_active() function
bool Pin_sensor::is_indeterminate() const
{
    return false;
}




