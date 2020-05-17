/*
 * loconet_sensor.cpp
 *
 *  Created on: Aug 10, 2018
 *      Author: ackpu
 */
#include <string.h>
#include "loconet_sensor.h"

namespace mr_signals {


/**
 * Loconet_sensor constructor
 *
 * Creates a concrete Loconet sensor with a name and a Loconet sensor address to capture
 * which observes a Loconet adapter to receive changes in state from the Loconet.
 *
 * @param name:         Name of the sensor to output on the debug console
 * @param address:      Loconet address of the sensor
 * @param ln_adapter:   Reference to an adapter to attach this sensor to
 */
Loconet_sensor::Loconet_sensor(const char *name, const Loconet_address address, Loconet_adapter_interface& ln_adapter) :
        address_(address)
{

    // Ensure this sensor is observing the Loconet adapter
    ln_adapter.attach_sensor(this);


//#pragma warning(disable: 4996)
    //strncpy(name_, name, ln_sensor_name_len);  // TODO: strlcpy?
    strlcpy(name_,name,sizeof(name_));

//    name_[ln_sensor_name_len] = '\0';

}


/**
 * Notification function for the Loconet adapter to inform sensors when a
 * new state is received
 *
 * @param address:  Loconet address of the sensor whose state has changed
 * @param state:    New state of the sensor
 * @return:         True if this is the sensor with passed address, false otherwise
 */
bool Loconet_sensor::notify(const Loconet_address address, const bool state)
{
    bool this_sensor = false;

    if(address == address_) {
        set_state(state);       // Sensor_base::set_state()
        this_sensor = true;
    }

    return this_sensor;
}

Loconet_address Loconet_sensor::get_address() const
{
    return address_;
}

const char* Loconet_sensor::get_name() const
{
    return name_;
}



}   // namespace mr_signals

