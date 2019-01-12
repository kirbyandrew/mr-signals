/*
 * apb_sensor.cpp
 *
 *  Created on: Nov 25, 2018
 *      Author: ackpu
 */

#include <apb_logic.h>
#include "algorithm.h"

using namespace mr_signals;

/*
 * class Simple_apb
{
public:
    Simple_apb(std::initializer_list<Sensor_interface *> const & protected_sensors);

    void loop();

protected:
    std::vector<Sensor_interface*> protected_sensors_;
    Sensor_base direction_left_sensor;
    Sensor_base direction_right_sensor;
};
 *
 */

Simple_apb::Simple_apb(std::initializer_list<Sensor_interface *> const & protected_sensors) :
        protected_sensors_(protected_sensors)
{
    // Leave the tumbdown_sensors in their default (indeterminate) state
}

void Simple_apb::loop()
{
    // First check that none of the sensors are indeterminate
    if (std::none_of(protected_sensors_.begin(),
                    protected_sensors_.end(),
                    [](Sensor_interface* sensor) {return sensor->is_indeterminate();})) {

        // Check for a completely empty block
        if (std::none_of(protected_sensors_.begin(),
                        protected_sensors_.end(),
                        [](Sensor_interface* sensor) {return sensor->is_active();})) {

            // No protected sensors are active, clear the tumble downs
            up_tumbledown_sensor.set_state(false);
            down_tumbledown_sensor.set_state(false);
        }
        else {  // At least one sensor is active, none are indeterminate

            if(protected_sensors_.front()->is_active() &&   // First block is occupied
                !down_tumbledown_sensor.is_active()) {      // Is not a train heading in the up direction
                                                            // (e.g. leaving rather than entering the protected blocks)

                // It appears that a train is entering the first block in the
                // down direction, so set the up direction tumbledown
                up_tumbledown_sensor.set_state(true);
            }

            if(protected_sensors_.back()->is_active() &&    // Last block is occupied
                !up_tumbledown_sensor.is_active()) {        // Is not a train heading in the down direction
                                                            // (e.g. leaving rather than entering the protected blocks)

                down_tumbledown_sensor.set_state(true);
            }

        }

    }   // Sensor indeterminate check
}


Sensor_interface& Simple_apb::down_tumbledown()
{
    return dynamic_cast<Sensor_interface&>(down_tumbledown_sensor);
}

Sensor_interface& Simple_apb::up_tumbledown()
{
    return dynamic_cast<Sensor_interface&>(up_tumbledown_sensor);
}
