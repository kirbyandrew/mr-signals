/*
 * loconet_sensor.h
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */


#ifndef SRC_LOCONET_LOCONET_SENSOR_H_
#define SRC_LOCONET_LOCONET_SENSOR_H_

#include "sensor_interface.h"
#include "loconet_adapter_interface.h"

namespace mr_signals {

/***
 * Concrete class for sensor states that are received over Loconet, e.g. from
 * OPC_INPUT_REP (0xB2) general sensor messages
 *
 * Inherited from the Sensor_base class, it provides an interface (sensor_notify)
 * to allow the state of the sensor to be modified by the Loconet adapter
 * class in use
 *
 * When the class is instantiated, a refernce to a Loconet adapter is passed
 * so that the sensor can be attached to the Loconet adapter's sensor list.
 * This is done in the constructor to ensure that any declared sensor
 * is always subscribed to updates from the loconet adapter
 *
 */
class Loconet_sensor : public Sensor_base {
public:

    Loconet_sensor(const char *name, const Loconet_address address, Loconet_adapter_interface& ln_adapter);

    /// Notifies the sensor that its state has been changed
    bool notify(const Loconet_address address, const bool state);

    /// Get the address assigned at constructor for this sensor
    Loconet_address get_address() const;

    // Get the name assigned in the constructor for this sensor
    const char *get_name() const;


private:
    /// Name of the sensor .  Char array more RAM efficient than std::string
    static const int ln_sensor_name_len = 5;
    char name_[ln_sensor_name_len+1];

    /// Loconet address of the sensor
    Loconet_address address_;
};


}

#endif /* SRC_LOCONET_LOCONET_SENSOR_H_ */
