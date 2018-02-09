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
    case dark:          // Close (turn off) the switch for dark & red
    case red:
        result = switch_1_.request_direction(switch_closed);
        break;

    case yellow:        // Throw (turn on) the switch for other valid aspects
    case green:
        result = switch_1_.request_direction(switch_thrown);
        break;

    case unknown:       // Take no action if the state is invalid or unknown
    default:
        break;

    }

    return result;
}


void Single_switch_head::loop()
{
    switch_1_.loop();
}







Single_switch_sensor_head::Single_switch_sensor_head(const char *name,
        Switch_interface& switch_1, Sensor_interface& sensor) :
        Single_switch_head(name,switch_1), sensor_(sensor)
{

}

/**
 * Allows the underlying head aspect to be set if the aspect requested is
 * dark, red or if the sensor is active for any other aspect
 *
 * @param aspect
 * @return true if the state of the head changes
 */
bool Single_switch_sensor_head::request_aspect(const Head_aspect aspect)
{

    if( (dark == aspect) ||
        (red  == aspect) ||
        (!sensor_.is_indeterminate() && true == sensor_.get_state()))
    {
        return Single_switch_head::request_aspect(aspect);
    }
    else
    {
        return false;
    }
}




