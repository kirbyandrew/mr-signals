/*
 * single_switch_head.cpp
 *
 *  Created on: Feb 3, 2018
 *      Author: ackpu
 */

#include "double_switch_head.h"

using namespace mr_signals;

Double_switch_head::Double_switch_head(const char* name,
        Switch_interface& switch_1,
        Switch_interface& switch_2) :
        Head_interface(name), switch_1_(switch_1), switch_2_(switch_2)
{

}

/// Set the two switch states in the combination described in the header
bool Double_switch_head::request_outputs(const Head_aspect aspect) {
    bool result = false;

    switch (aspect) {
    case dark:
        if (true == switch_1_.request_direction(switch_closed)) {
            result = switch_2_.request_direction(switch_closed);
        }
        break;

    case green:
        if (true == switch_1_.request_direction(switch_thrown)) {
            result = switch_2_.request_direction(switch_closed);
        }
        break;

    case yellow:
        if (true == switch_1_.request_direction(switch_thrown)) {
            result = switch_2_.request_direction(switch_thrown);
        }
        break;

    case red:
        if (true == switch_1_.request_direction(switch_closed)) {
            result = switch_2_.request_direction(switch_thrown);
        }
        break;

    case unknown:
    default:
        // result = false;
        break;

    }

    return result;
}


void Double_switch_head::loop()
{
    switch_1_.loop();
    switch_2_.loop();
}


