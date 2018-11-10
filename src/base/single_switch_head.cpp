/*
 * single_switch_head.cpp
 *
 *  Created on: Feb 3, 2018
 *      Author: ackpu
 */

#include "single_switch_head.h"

using namespace mr_signals;


Single_switch_head::Single_switch_head(const char* name,
        Switch_interface& switch_1) :
        Head_interface(name), switch_1_(switch_1)
{

}


bool Single_switch_head::request_outputs(const Head_aspect aspect)
{
    bool result = false;

    switch (aspect) {
    case Head_aspect::dark:          // Close (turn off) the switch for dark & red
    case Head_aspect::red:
        result = switch_1_.request_direction(Switch_direction::closed);
        break;

    case Head_aspect::yellow:        // Throw (turn on) the switch for other valid aspects
    case Head_aspect::green:
        result = switch_1_.request_direction(Switch_direction::thrown);
        break;

    case Head_aspect::unknown:       // Take no action if the state is invalid or unknown
    default:
        break;

    }

    return result;
}


void Single_switch_head::loop()
{
    switch_1_.loop();
}



