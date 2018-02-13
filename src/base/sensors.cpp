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



Inverted_sensor::Inverted_sensor(Sensor_interface& sensor) :
        sensor_(sensor)
{
}

/**
 * Returns the inverted state of the sensor
 *
 * While get_state() should not be called if a sensor is indeterminate,
 * the return is forced to false to protect against that case
 * @return
 */bool Inverted_sensor::get_state()
{
    if(sensor_.is_indeterminate()) {
        return false;
    }
    else {
        return !sensor_.get_state();
    }
}

bool Inverted_sensor::is_indeterminate()
{
    return sensor_.is_indeterminate();
}




