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


class Loconet_sensor : public Sensor_base {
public:
    Loconet_sensor(const char *name, const unsigned int address, Loconet_adapter_interface& ln_adapter) :
        address_(address) {ln_adapter.attach_sensor(this); }


    bool loop() override { return false; }

    bool sensor_notify(const unsigned int address, const bool state);

    Loconet_address get_address() const { return address_; }

//    void debugPrint() {}

private:
    Loconet_address address_;

};


}

#endif /* SRC_LOCONET_LOCONET_SENSOR_H_ */
