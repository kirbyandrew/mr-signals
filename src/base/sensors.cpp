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
    return (bool) state_;
}

bool Sensor_base::is_indeterminate()
{
    return (bool) indeterminate_;
}

bool Sensor_base::set_state(const bool state)
{
    bool changed = false;


    // If the sensor state is indeterminate, always mark as a change
    if(indeterminate_) {
        changed = true;
    }

    // Check if the state of the sensor changes
    if (state != (bool) state_) {
        state_ = (true==state) ? 1 : 0;
        changed = true;
    }

    // Once set, the state is no longer indeterminate
    indeterminate_ = 0;

    return changed;
}



