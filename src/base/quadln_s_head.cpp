/*
 * quadln_s_head.cpp
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#include <quadln_s_head.h>

using namespace mr_signals;


Quadln_s_head::Quadln_s_head(const char* name,
        Switch_interface& switch_1,
        Switch_interface& midpoint_switch) :
        Double_switch_head(name, switch_1, midpoint_switch)
{

}

/**
 * Set the end-point (green/yellow) switch before clearing the midpoint switch,
 * as the QuadLN_S starts travelling to the value of switch_1_ as soon as
 * the midpoint (switch_2_) is released.
 *
 * For yellow, just set the midpoint (switch_2_) as it overrides switch_1_'
 * s value in the QuadLN_S
 */
bool Quadln_s_head::request_outputs(const Head_aspect aspect)
{
    // Assume failure as the default
    bool result = false;

    switch (aspect) {
    case Head_aspect::green:
        if (true == switch_1_.request_direction(Switch_direction::thrown)) {
            result = switch_2_.request_direction(Switch_direction::closed);
        }
        break;

    case Head_aspect::yellow:
        result = switch_2_.request_direction(Switch_direction::thrown);
        break;

    case Head_aspect::red:
        if (true == switch_1_.request_direction(Switch_direction::closed)) {
            result = switch_2_.request_direction(Switch_direction::closed);
        }
        break;

    case Head_aspect::unknown:
    case Head_aspect::dark:      // Not supported by QuadLN_S
    default:
        // result = false;
        break;

    }

    return result;
}


