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


class Red_head_sensor : public Sensor_interface  {
public:
    Red_head_sensor(Head_interface& head) : head_(head) {}

    bool get_state() override
    {
        if (Head_aspect::red == head_.get_aspect()) {
            return true;
        }
        return false;
    }

    bool is_indeterminate() override {
        return false;
    }

//    ~Red_head_sensor() {}


private:
Head_interface& head_;
};



class Head_aspect_sensor : public Sensor_interface  {
public:
    Head_aspect_sensor(const Head_interface& head, const Head_aspect active_aspect) : head_(head),active_aspect_(active_aspect) {}

    bool get_state() override
    {
        if (active_aspect_ == head_.get_aspect()) {
            return true;
        }
        return false;
    }

    bool is_indeterminate() override {
        return false;
    }

//    ~Red_head_sensor() {}


private:
    const Head_interface& head_;
    Head_aspect active_aspect_;
};


}



#endif /* SRC_HELPERS_H_ */
