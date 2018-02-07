/*
 * single_switch_head.cpp
 *
 *  Created on: Feb 3, 2018
 *      Author: ackpu
 */

#include <string.h>
#include "double_switch_head.h"

using namespace mr_signals;

Double_switch_head::Double_switch_head(const char* name,
        Switch_interface& switch_1,
        Switch_interface& switch_2) :
        switch_1_(switch_1), switch_2_(switch_2), aspect_(unknown), held_(false)
{

    strncpy(name_, name, head_name_len);
    name_[head_name_len] = '\0';
}

Head_aspect Double_switch_head::get_aspect()
{
    return aspect_;
}

bool Double_switch_head::set_aspect(const Head_aspect aspect)
{
    bool result = false;

    if (!is_held()) {
        if(aspect != aspect_) {
            // Request is to change the aspect of the head
            switch (aspect) {
            case dark:
                if (true == switch_1_.request_direction(switch_closed));
                result = switch_2_.request_direction(switch_closed);
                break;

            case green:
                if (true == switch_1_.request_direction(switch_thrown));
                result = switch_2_.request_direction(switch_closed);
                break;

            case yellow:
                if (true == switch_1_.request_direction(switch_thrown));
                result = switch_2_.request_direction(switch_thrown);
                break;

            case red:
                if (true == switch_1_.request_direction(switch_closed));
                result = switch_2_.request_direction(switch_thrown);
                break;

            case unknown:
            default:
                // result = false;
                break;

            }

            if(result)
            {
                aspect_ = aspect;
  //              Debug << "Setting Head to "<< aspect_ << "\n";
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
        if(aspect == aspect_) {
            result = true;
        }
    }

    return result;
}

const char* Double_switch_head::get_name()
{
    return name_;
}

bool Double_switch_head::loop()
{
    bool ret = false;

    switch_1_.loop();
    switch_2_.loop();

    return ret;
}

void Double_switch_head::set_held(const bool held)
{
    held_ = (aspect_ != unknown && true==held) ? 1 : 0;
}


bool Double_switch_head::is_held() {
    return held_ ? true : false;
}



