/*
 * helpers.h
 *
 *  Created on: Feb 25, 2018
 *      Author: ackpu
 */

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

#include "sensor_interface.h"
#include "base/logic_interface.h"
#include "logic_collection.h"
#include "base/head_interface.h"

namespace mr_signals {

/**
 * Sensor that returns active if an observed head has a red aspect
 *
 * Primary use case is to act as a protected sensor for a head that needs
 * to show a red aspect when the head that it is protecting is red (in
 * basic RYG logic, this would result in a Yellow Aspect)
 */
class Red_head_sensor : public Sensor_interface  {
public:
    Red_head_sensor(Head_interface& head) : head_(head) {}

    bool is_active() override
    {
        if (Head_aspect::red == head_.get_aspect()) {
            return true;
        }
        return false;
    }

    bool is_indeterminate() const override {
        if(Head_aspect::unknown == head_.get_aspect()) {
            return true;
        }
        else {
            return false;
        }
    }

private:
    Head_interface& head_;
};


/**
 * Sensor that returns active if an observed head has a specified aspect
 *
 * To be used when a simpler Red_head_sensor does not meet the need.
 */
class Head_aspect_sensor : public Sensor_interface  {
public:
    Head_aspect_sensor(const Head_interface& head, const Head_aspect active_aspect) : head_(head),active_aspect_(active_aspect) {}

    bool is_active() override
    {
        if (active_aspect_ == head_.get_aspect()) {
            return true;
        }
        return false;
    }

    bool is_indeterminate() const override {
        return false;
    }


private:
    const Head_interface& head_;
    Head_aspect active_aspect_;
};


/**
 * Two_lever_interlock
 *
 * Logic to Inter-lock two levers that control opposing routes so
 * that both routes cannot be cleared at the same time but not
 * cause an already cleared route to be blocked as would occur
 * if the routes' Logic simply had the opposing levers in their
 * protected sensor list.
 * Instead this class provides additional sensors that can be
 * added to each route's Logic protected sensors to prevent
 * the route being cleared.
 *
 * Both levers must be returned to normal to clear the locks.
 *
 * Example use:
 *
 * Sensor_base sensor_1;
 * Sensor_base sensor_2;
 * Logic_collection logic(1);
 *
 * Two_lever_interlock<Sensor_base, &sensor_1, Sensor_base, &sensor_2> sensor_1_2_interlock(logic);
 *
 */

// TODO: Add initializer list of sensors to add to checks, e.g. for 1B to lock 24
template<class T1, T1 *sensor_1, class T2, T2 *sensor_2>
class Two_lever_interlock : public Logic_interface {

public:

    Two_lever_interlock (Logic_collection& collection) :
        Logic_interface(collection) {

        // Initialize sensors to inactive so that they don't cause
        // any logic problems by staying indeterminate
        lock_first.set_state(false);
        lock_second.set_state(false);
    }

    void loop() override {
        if(!sensor_1->is_indeterminate() && !sensor_2->is_indeterminate()) {

            if(sensor_1->is_active() && !lock_first.is_active()) {
                // If the first sensor is active, lock the second
                lock_second.set_state(true);
            }
            else if(sensor_2 -> is_active() && !lock_second.is_active()) {
                // If the second sensor is active, lock the first
                lock_first.set_state(true);
            }
            else if(!sensor_1->is_active() && !sensor_2->is_active()){
                // Require both sensors to go inactive to release the locks
                lock_first.set_state(false);
                lock_second.set_state(false);
            }
        }
    }

    Sensor_base lock_first;
    Sensor_base lock_second;
};

}



#endif /* SRC_HELPERS_H_ */
