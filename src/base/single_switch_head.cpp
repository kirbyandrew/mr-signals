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


bool Single_switch_head::request_aspect(const Head_aspect aspect)
{
    bool result = false;

    if (!is_held()) {

        if (aspect != get_aspect()) {

            // Request is to change the aspect of the head
            switch (aspect) {
            case dark:
            case red:
                // Close (turn off) the switch for dark
                result = switch_1_.request_direction(switch_closed);
                break;

            case yellow:
            case green:
                // Throw (turn on) the switch for other valid aspects
                result = switch_1_.request_direction(switch_thrown);
                break;

            case unknown:
            default:
                // Take no action if the state is invalid or unknown
                break;

            }   // switch(aspect)

            if (true == result) {
                // Update internal state
                set_aspect(aspect);
            }
        } else {
            // If Aspect is already the same, report a successful setting
            // of the aspect
            result = true;
        }
    } else {

        // If the head's aspect is being held, only report success if the
        // requested aspect is already set
        if(get_aspect()==aspect) {
            result = true;
        }
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




