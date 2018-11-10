/*
 * helpers.h
 *
 *  Created on: Feb 25, 2018
 *      Author: ackpu
 */

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

#include "sensor_interface.h"
#include "head_interface.h"

namespace mr_signals {

/**
 * Sensor that returns active if an observed head has a red aspect
 *
 * Primary use case is to act as a protected sensor for a head that needs
 * to show a red aspect when the head that it is protecting is red (in
 * basic RYG logic, this would result in a Yellow Aspect)
 */
class Red_head_sensor : public Sensor_interface  {
public:
    Red_head_sensor(Head_interface& head) : head_(head) {}

    bool is_active() override
    {
        if (Head_aspect::red == head_.get_aspect()) {
            return true;
        }
        return false;
    }

    bool is_indeterminate() const override {
        return false;
    }

private:
    Head_interface& head_;
};


/**
 * Sensor that returns active if an observed head has a specified aspect
 *
 * To be used when a simpler Red_head_sensor does not meet the need.
 */
class Head_aspect_sensor : public Sensor_interface  {
public:
    Head_aspect_sensor(const Head_interface& head, const Head_aspect active_aspect) : head_(head),active_aspect_(active_aspect) {}

    bool is_active() override
    {
        if (active_aspect_ == head_.get_aspect()) {
            return true;
        }
        return false;
    }

    bool is_indeterminate() const override {
        return false;
    }


private:
    const Head_interface& head_;
    Head_aspect active_aspect_;
};


}



#endif /* SRC_HELPERS_H_ */
