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


bool Double_switch_head::request_aspect(const Head_aspect aspect)
{
    bool result = false;

    if (!is_held()) {
        if(aspect != get_aspect()) {
            // Request is to change the aspect of the head
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

            if(result)
            {
                set_aspect(aspect);
            }
        }
        else
        {
            // If Aspect is already the same, leave it
            result = true;
        }
    } else {    // if (!is_held())

        // If the head's aspect is being held, only report success if the
        // requested aspect is already set
        if(get_aspect() == aspect) {
            result = true;
        }
    }

    return result;
}


void Double_switch_head::loop()
{
    switch_1_.loop();
    switch_2_.loop();
}


