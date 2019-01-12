/*
 * apb_sensor.h
 *
 *  Created on: Nov 23, 2018
 *      Author: ackpu
 */

#ifndef SRC_BASE_APB_SENSOR_H_
#define SRC_BASE_APB_SENSOR_H_

#include <vector>
#include "base/logic_interface.h"
#include "sensor_interface.h"

namespace mr_signals
{


/**
 * A very simple APB logic implementation.
 *
 * The APB section is represented by a series of sensor inputs, listed in
 * the order matching their physical implementation on the model railway.
 *
 * The logic maintains two 'tumbledown' sensors, each of which goes active
 * when a train first enters the block of protected sensors in a given
 * direction.  These sensors can then be included in all signals protecting
 * the opposing direction of travel within the APB section.
 *
 * The sensors are then both cleared (set inactive) when none of the protected
 * sensors are active.
 *
 * The sensors are enumerated in the following directions, with aliased access
 * to the sensors in the following tumble-down directions
 *   Down   (Up/Down roads)
 *   South  (North/South roads)
 *   West   (East/West routes)
 *
 * The logic will not work correctly if a train is in the block when the
 * program is started (powered on).
 */
class Simple_apb : public Logic_interface
{
public:
    Simple_apb(std::initializer_list<Sensor_interface *> const & protected_sensors);

    void loop() override;

    Sensor_interface& down_tumbledown();
    Sensor_interface& up_tumbledown();

protected:
    std::vector<Sensor_interface*> protected_sensors_;
    Sensor_base down_tumbledown_sensor;
    Sensor_base up_tumbledown_sensor;
};

// Use otherwise regrettable #defines due to the lack of class function aliases
#define south_tumbledown down_tumbledown
#define north_tumbledown up_tumbledown

#define west_tumbledown down_tumbledown
#define east_tumbledown up_tumbledown

}



#endif /* SRC_BASE_APB_SENSOR_H_ */
