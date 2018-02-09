/*
 * quadln_s_head.cpp
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#include "quadln_s_head.h"

using namespace mr_signals;

Quadln_s_head::Quadln_s_head(const char* name,
        Switch_interface& switch_1,
        Switch_interface& midpoint_switch) :
        Head_interface(name), switch_1_(switch_1), midpoint_switch_(midpoint_switch)
{

}

/**
 * Set the end-point (green/yellow) switch before clearing the midpoint switch,
 * as the QuadLN_S starts travelling to the value of switch_1_ as soon as
 * the midpoint is released.
 *
 * For yellow, just set the midpoint as it overrides switch_1_'s value in the
 * QuadLN_S
 */
bool Quadln_s_head::request_outputs(const Head_aspect aspect)
{
    // Assume failure as the default
    bool result = false;

    switch (aspect) {
    case green:
        if (true == switch_1_.request_direction(switch_thrown)) {
            result = midpoint_switch_.request_direction(switch_closed);
        }
        break;

    case yellow:
        result = midpoint_switch_.request_direction(switch_thrown);
        break;

    case red:
        if (true == switch_1_.request_direction(switch_closed)) {
            result = midpoint_switch_.request_direction(switch_closed);
        }
        break;

    case unknown:
    case dark:      // Not supported by QuadLN_S
    default:
        // result = false;
        break;

    }

    return result;
}


void Quadln_s_head::loop()
{
    switch_1_.loop();
    midpoint_switch_.loop();
}



