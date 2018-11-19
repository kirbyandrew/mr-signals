/*
 * pin_sensor.h
 *
 *  Created on: Nov 18, 2018
 *      Author: ackpu
 */

#ifndef SRC_PIN_SENSOR_H_
#define SRC_PIN_SENSOR_H_

#include <cstdlib>
#include "sensor_interface.h"

namespace mr_signals {

class Pin_sensor : public Sensor_base {
public:
    Pin_sensor(uint8_t pin);

    bool is_active() override;

    bool is_indeterminate() const override;

protected:
    uint8_t pin_;
};

}

#endif /* SRC_PIN_SENSOR_H_ */


