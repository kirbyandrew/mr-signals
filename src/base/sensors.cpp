/*
 * sensors.cpp
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */

#include <sensor_interface.h>

using namespace mr_signals;

bool Sensor_base::get_state()
{
    return state_;
}

bool Sensor_base::is_indeterminate()
{
    return indeterminate_;
}

bool Sensor_base::set_state(bool state)
{
    bool changed = false;

    if (state != state_) {
        state_ = state;
        changed = true;
    }

    indeterminate_ = false;

    return changed;
}



