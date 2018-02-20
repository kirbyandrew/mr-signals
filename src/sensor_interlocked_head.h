/*
 * sensor_dependant_head.h
 *
 *  Created on: Feb 17, 2018
 *      Author: ackpu
 */

#ifndef SRC_SENSOR_INTERLOCKED_HEAD_H_
#define SRC_SENSOR_INTERLOCKED_HEAD_H_

#include "base/head_interface.h"
#include "sensor_interface.h"
#include "base/switch_interface.h"

namespace mr_signals {

class Sensor_interlocked_head : public Head_interface
{
public:

    // TODO: What a mess - looks like Head_interface is far too implemented,
    // should be much more abstract with a base class inbetween
    Sensor_interlocked_head(Head_interface& head,
            Sensor_base& sensor) :
                Head_interface(""), head_(head), sensor_(sensor) {
    }


    /// Specialized output functionality for the sensor interlock
    /// The head will only go to a non-dark/red aspect if the
    /// interlocked sensor is active
    bool request_outputs(const Head_aspect aspect) override
    {
        if (    (Head_aspect::dark == aspect) ||
                (Head_aspect::red == aspect) ||
                (!sensor_.is_indeterminate() && true == sensor_.get_state())) {

            return head_.request_aspect(aspect);
        }
        else {
            return false;
        }
    }

    void loop() override
    {
        // Handle startup error case.  For an interlocked head, we can
        // assume we want it to start at red unless we see the lever
        // go active, and if the aspect is not red many of the logic
        // algorithms won't hold the head which may lead to unexpected
        // behavior due to some of the differences in the behaviour
        // of the request_outputs() for this head.
        // For example, if this is used for a callon button with the
        // Interlocked_ryg_logic() and the lever is thrown at startup,
        // but the call-on button is not pressed, the logic will attempt
        // to set this head to green (which will be rejected by
        // request_outputs() above as the sensor is not active), leaving the
        // head in aspect ::unknown, and meaning that the logic will not
        // hold this head at red.
        if(Head_aspect::unknown == head_.get_aspect()) {
            head_.request_aspect(Head_aspect::red);
        }

        head_.loop();
    }


    /// Get the current aspect of the head
    Head_aspect get_aspect() override
    {
        return head_.get_aspect();
    }

    /// Get the name of the head
    const char* get_name() override
    {
        return head_.get_name();
    }

    /// 'Lock' the current aspect of the head. Ignored if the state is unknown
    void set_held(const bool held) override
    {
        head_.set_held(held);
    }

    /// Indicate whether the head's aspect is currently locked
    bool is_held() override
    {
        return head_.is_held();
    }



private:
    Head_interface& head_;
    Sensor_base& sensor_;
};


}

#endif /* SRC_SENSOR_INTERLOCKED_HEAD_H_ */
