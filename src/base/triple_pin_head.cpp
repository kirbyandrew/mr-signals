/*
 * triple_pin_head.cpp
 *
 *  Created on: Nov 18, 2018
 *      Author: ackpu
 */

#include <stdint.h>
#include "triple_pin_head.h"


#ifndef ARDUINO
#include "arduino_mock.h"   // pinMode(), digitalWrite() for unit tests not on Arduino
#else
#include "Arduino.h"
#endif


using namespace mr_signals;

/// Initialize the passed pins to outputs and ensure a dark aspect is shown.
Triple_pin_head::Triple_pin_head(const char* name, uint8_t green_pin, uint8_t yellow_pin, uint8_t red_pin) :
        Head_interface(name), green_pin_(green_pin), yellow_pin_(yellow_pin), red_pin_(red_pin)
{
    pinMode(green_pin_,OUTPUT);
    pinMode(yellow_pin_,OUTPUT);
    pinMode(red_pin_,OUTPUT);

    request_aspect(Head_aspect::dark);
}


/// Ensure that only the single pin associated with the requested aspect is high
bool Triple_pin_head::request_aspect(const Head_aspect aspect) {

    bool result = true;

    digitalWrite(green_pin_,LOW);
    digitalWrite(yellow_pin_,LOW);
    digitalWrite(red_pin_,LOW);


    switch (aspect) {
    case Head_aspect::dark:
        // All LEDS already off, leave
        break;

    case Head_aspect::green:
        digitalWrite(green_pin_,HIGH);
        break;

    case Head_aspect::yellow:
        digitalWrite(yellow_pin_,HIGH);
        break;

    case Head_aspect::red:
        digitalWrite(red_pin_,HIGH);
        break;

    case Head_aspect::unknown:
    default:
        result = false;
        break;

    }

    return (result);
}


