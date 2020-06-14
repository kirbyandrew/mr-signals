/*
 * apb_sensor.cpp
 *
 *  Created on: Nov 25, 2018
 *      Author: ackpu
 */

#include <cstddef>      // size_t
#include <apb_logic.h>
#include "algorithm.h"

using namespace mr_signals;



Simple_apb::Simple_apb(Logic_collection& collection, std::initializer_list<Sensor_interface *> const & protected_sensors) :
        Logic_interface(collection), protected_sensors_(protected_sensors)
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






Full_apb::Full_apb(Logic_collection& collection, std::initializer_list<Sensor_interface *> const & protected_sensors) :
        Logic_interface(collection), protected_sensors_(protected_sensors) {


    // Check protection against an empty initializer list being passed
    // The tumbledown sensor get functions have to return something, so
    // at least one sensor in each is always allocated
    num_sensors_ = (protected_sensors_.size() > 0) ? protected_sensors_.size() : 1;


    down_tumbledown_sensors_.reserve(num_sensors_);
    up_tumbledown_sensors_.reserve(num_sensors_);

    for(uint8_t i=0;i < num_sensors_; i++) {
        down_tumbledown_sensors_.push_back(new Sensor_base());
        up_tumbledown_sensors_.push_back(new Sensor_base());
    }

}

#include <iostream>

void Full_apb::loop() {

    // See
    // http://www.lundsten.dk/us_signaling/abs_apb/index.html
    // Protection of following trains

    // Basic logic
    // In the opposite direction of travel of trains within the block...
    // Iterate over all sensors to find the 'last' train (occupied block)
    // All tumbledown sensors before that are active
    // None after the last train are active
    // Clear all tumbledowns if no sensors are active

    /*
     * x             x             x             x             x
     * | ---- x ---- | ---- x ---- | ---- x ---- | ---- x ---- |---- x ----|
     *               x             x             x             x           x
     *
     *
     *===>
     * x             x             x             x             x
     * | ---- A ---- | ---- x ---- | ---- x ---- | ---- x ---- |---- x ----|
     *               A             A             A             A           A
     *
     * (inverse)
     * A             A             A             A             A         <====
     * | ---- A ---- | ---- x ---- | ---- x ---- | ---- x ---- |---- x ----|
     *               x             x             x             x           x
     *
     *
     *
     *           =======>
     * x             x             x             x             x
     * | ---- A ---- | ---- A ---- | ---- x ---- | ---- x ---- |---- x ----|
     *               A             A             A             A           A
     *
     *
     *                   =======>
     * x             x             x             x             x
     * | ---- x ---- | ---- A ---- | ---- x ---- | ---- x ---- |---- x ----|
     *               x             A             A             A           A
     *
     *( inverse)
     *                                              <========
     * A             A             A             A             x
     * | ---- x ---- | ---- A ---- | ---- x ---- | ---- x ---- |---- x ----|
     *               x             x             x             x           x
     *
     *
     *
     *                          =======>
     * x             x             x             x             x
     * | ---- x ---- | ---- A ---- | ---- A ---- | ---- x ---- |---- x ----|
     *               x             A             A             A           A
     *
     *
     *                                 =======>
     * x             x             x             x             x
     * | ---- x ---- | ---- x ---- | ---- A ---- | ---- x ---- |---- x ----|
     *               x             x             A             A           A
     *
     *
     *                                       =======>
     * x             x             x             x             x
     * | ---- x ---- | ---- x ---- | ---- A ---- | ---- A ---- |---- x ----|
     *               x             x             A             A           A
     *
     *
     * =======>                                =======>
     * x             x             x             x             x
     * | ---- A ---- | ---- x ---- | ---- A ---- | ---- x ---- |---- x ----|
     *               A             A             A             A           A
     *
     */

    /*  //If [0] & no up tumbledowns
     *  //    Set all up tumbledowns
     *
     *  //If [-1] & no down tumbledowns
     *  //    Set all down tumbledowns
     *
     * If any up tumbledowns...
     *   tumbledown_up = false
     *   for sensor0 to sensorN-1
     *      if sensorx is clear & ! tumbledown_up:
     *         up_tumbledown[x].clear
     *      else
     *         tumbledown_up = true
     *         up_tumbledown[x].set
     * Else if protected_sensor[0]  // No up tumbledowns
     *   Set all up tumbledowns``// train entering in down direction
     *
     * If any down tumbledowns...
     *   tumbledown_down = false
     *   for sensorN-1 to sensor0
     *      if sensorx is clear & ! tumbledown_down:
     *         down_tumbledown[x].clear
     *      else
     *         tumbledown_down = true
     *         down_tumbledown[x].set
     * Else if protected_sensors[-1] // No down tumbledowns
     *   Set all down tumbledowns  // train entering in up direction
     *
     *
     */


    // Do nothing until the state of the sensors is known
    if (std::none_of(protected_sensors_.begin(),
                    protected_sensors_.end(),
                    [](Sensor_interface* sensor) {return sensor->is_indeterminate();})) {


        if (std::none_of(protected_sensors_.begin(),
                        protected_sensors_.end(),
                        [](Sensor_interface* sensor) {return sensor->is_active();})) {

            std::cout << "No sensors active, clear all tumbledowns\n";

            // All sensors are clear, clear all tumbledowns
            for(auto const& tumbledown : up_tumbledown_sensors_) {
                tumbledown -> set_state(false);
            }

            for(auto const& tumbledown : down_tumbledown_sensors_) {
                tumbledown -> set_state(false);
            }
        }
        else {

            // At least one sensor is active; run the APB logic

            if (std::any_of(up_tumbledown_sensors_.begin(),
                            up_tumbledown_sensors_.end(),
                            [](Sensor_interface* sensor) {return sensor->is_active();})) {

                // There is an active up tumbledown sensor; run across the protected
                // sensors to determine which ones should be set and which not

                std::cout << "Up tumbledowns are active\n";

                bool set_all_subsequent_tumbledowns = false;

                // Loop through sensors in the 'down' direction (from 0 to n-1)
                for(std::vector<Sensor_interface*>::size_type i = 0; i != num_sensors_; i++) {
                    if(!protected_sensors_[i]->is_active() && !set_all_subsequent_tumbledowns) {
                        // Block is clear, clear the corresponding protecting tumbledown
                        up_tumbledown_sensors_[i]->set_state(false);
                    }
                    else {
                        // Block is not clear; set tumbledown and record this to set
                        // all subsequent ones
                        set_all_subsequent_tumbledowns = true;
                        up_tumbledown_sensors_[i]->set_state(true);
                    }
                }
            }
/*
            else if (protected_sensors_[0]->is_active()) {


                std::cout << "Up tumbledowns are inactive, sensor[0] active, set up tumbledowns\n";


                // There are no up tumbledowns, but the first block in the down direction
                // is active, so assume a train is entering in the down direction
                // and set the up tumbledowns
                for(auto const& tumbledown: up_tumbledown_sensors_) {
                    tumbledown->set_state(true);
                }
            }
*/
            else if (protected_sensors_[num_sensors_-1]->is_active()) {

                std::cout << "Up tumbledowns are inactive, sensor[n-1] active, set down tumbledowns\n";


                // There are no down tumbledowns actives, but the first block in the up direction
                // is active, so assume a train is entering in the up direction
                // and set the down tumbledowns
                for(auto const& tumbledown: down_tumbledown_sensors_) {
                    tumbledown->set_state(true);
                }
            }



            if (std::any_of(down_tumbledown_sensors_.begin(),
                            down_tumbledown_sensors_.end(),
                            [](Sensor_interface* sensor) {return sensor->is_active();})) {

                std::cout << "Down tumbledowns are active\n";


                // There is an active down tumbledown sensor; run across the protected
                // sensors to determine which ones should be set and which not
                bool set_all_subsequent_tumbledowns = false;

                // Loop through sensors in the 'down' direction (from 0 to n-1)
                for(int i = num_sensors_-1; i >= 0 ; --i) {
                    if(!protected_sensors_[i]->is_active() && !set_all_subsequent_tumbledowns) {
                        // Block is clear, clear the corresponding protecting tumbledown
                        down_tumbledown_sensors_[i]->set_state(false);
                    }
                    else {
                        // Block is not clear; set tumbledown and record this to set
                        // all subsequent ones
                        set_all_subsequent_tumbledowns = true;
                        down_tumbledown_sensors_[i]->set_state(true);
                    }
                }


            }
/*
            else if (protected_sensors_[num_sensors_-1]->is_active()) {

                std::cout << "Down tumbledowns are inactive, sensor[n-1] active, set down tumbledowns\n";


                // There are no down tumbledowns actives, but the first block in the up direction
                // is active, so assume a train is entering in the up direction
                // and set the down tumbledowns
                for(auto const& tumbledown: down_tumbledown_sensors_) {
                    tumbledown->set_state(true);
                }
            }
*/
            else if (protected_sensors_[0]->is_active()) {


                std::cout << "Down tumbledowns are inactive, sensor[0] active, set up tumbledowns\n";


                // There are no up tumbledowns, but the first block in the down direction
                // is active, so assume a train is entering in the down direction
                // and set the up tumbledowns
                for(auto const& tumbledown: up_tumbledown_sensors_) {
                    tumbledown->set_state(true);
                }
            }

        }
    }
    else {
        // At least one sensor indeterminate; skip
    }

}


Sensor_interface& Full_apb::down_tumbledown_num(uint8_t num) {
    size_t idx = num < down_tumbledown_sensors_.size() ? num : 0;

    // Can't cast a pointer to a reference directly; so use intermediate local pointer
    Sensor_interface *sensor = static_cast<Sensor_interface*>(down_tumbledown_sensors_[idx]);
    return *sensor;
}

Sensor_interface& Full_apb::up_tumbledown_num(uint8_t num) {
    size_t idx = num < up_tumbledown_sensors_.size() ? num : 0;

    // Can't cast a pointer to a reference directly; so use intermediate local pointer
    Sensor_interface *sensor = static_cast<Sensor_interface*>(up_tumbledown_sensors_[idx]);
    return *sensor;
}

