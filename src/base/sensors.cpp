/*
 * sensors.cpp
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */

#include <sensor_interface.h>

using namespace mr_signals;

bool Sensor_base::is_active()
{
    if(is_indeterminate()) {
        return false;
    }
    else {
        return (bool) state_;
    }
}

bool Sensor_base::is_indeterminate() const
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
        state_ = (true==state) ? sensor_active : sensor_inactive;
        changed = true;
    }

    // Once set, the state is no longer indeterminate
    indeterminate_ = sensor_inactive;

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
 */
bool Inverted_sensor::is_active()
{
    if(sensor_.is_indeterminate()) {
        return false;
    }
    else {
        return !sensor_.is_active();
    }
}

bool Inverted_sensor::is_indeterminate() const
{
    return sensor_.is_indeterminate();
}




