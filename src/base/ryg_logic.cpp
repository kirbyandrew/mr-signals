/*
 * masts.cpp
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#include <ryg_logic.h>
#include "algorithm.h"

using namespace mr_signals;


// TODO To test in Arduino
// - cost of passing initializer list
// comparison of individual classes for 'head_logic' rather than masts that collect it


Simple_ryg_logic::Simple_ryg_logic(Head_interface& head,
        Head_interface& protected_head,
        std::initializer_list<Sensor_interface *> const & protected_sensors) :
        head_(head), protected_head_(&protected_head), protected_sensors_(
                protected_sensors)
{
}

Simple_ryg_logic::Simple_ryg_logic(Head_interface& head,
        std::initializer_list<Sensor_interface *> const & protected_sensors) :
        head_(head), protected_head_(nullptr), protected_sensors_(
                protected_sensors)
{
}

/**
 * Aspect logic for a 3 aspect (red, yellow & green) head that protects
 * sensors and may protect another head
 *
 * If any protected sensor is active, set the aspect to red
 * If no protected sensors are active, but the protected head is red, set the
 * aspect to yellow
 * If no protected sensor is active, and the protected head is not red, set the aspect to green
 *
 * If the state of any protected sensor is unknown, delay processing until the
 * state is known to avoid potential race conditions
 *
 */
void Simple_ryg_logic::loop()
{
    head_.loop();

    // Check that the state of all protected sensors are known
    if (std::none_of(protected_sensors_.begin(),
                    protected_sensors_.end(),
                    [](Sensor_interface* sensor) {return sensor->is_indeterminate();})) {

        // No sensors are indeterminate, process the head's setting
        Head_aspect aspect = Head_aspect::unknown;

        // First check the protected sensors
        if (std::any_of(protected_sensors_.begin(),
                        protected_sensors_.end(),
                        [](Sensor_interface* sensor) {return sensor->is_active();})) {

            // A protected block is occupied, set the head red
            aspect = Head_aspect::red;
        }
        else {
            // Since the protected blocks are clear, check the protected head
            // and determine a green or yellow aspect

            bool protected_head_stop = false;

            if (nullptr != protected_head_) {
                if (Head_aspect::red == protected_head_->get_aspect()) {
                    protected_head_stop = true;
                }
            }

            if (protected_head_stop) {
                // Protected signal is at stop, so set this head to Caution/yellow
                aspect = Head_aspect::yellow;
            } else {
                // Both the protected blocks are clear and the protected signal is
                // not at stop (or doesn't exist); set a Proceed indication
                aspect = Head_aspect::green;
            }
        }

        // If the new aspect determined for this head differs from its current
        // value, request that the head change
        if (aspect != head_.get_aspect()) {
            if (head_.request_aspect(aspect) == true) {
    //                Debug << F("Mast ") << name_.c_str() << head.head->GetName() << F(" changed to ") << GetAspectString(head.head -> GetAspect()) << F("\n");
            }
        }
    }
}



Interlocked_ryg_logic::Interlocked_ryg_logic(Head_interface& head,
        Head_interface& protected_head, Sensor_interface& lever,
        std::initializer_list<Sensor_interface *> const & protected_sensors) :
        Simple_ryg_logic(head, protected_head, protected_sensors),
        lever_(lever), automated_lever_(nullptr)
{
}


Interlocked_ryg_logic::Interlocked_ryg_logic(Head_interface& head,
        Head_interface& protected_head, Sensor_interface& lever,
        Sensor_interface& automated_lever,
        std::initializer_list<Sensor_interface *> const & protected_sensors) :
        Simple_ryg_logic(head, protected_head, protected_sensors),
        lever_(lever), automated_lever_(&automated_lever)
{
}


Interlocked_ryg_logic::Interlocked_ryg_logic(Head_interface& head,
        Sensor_interface& lever,
        std::initializer_list<Sensor_interface *> const & protected_sensors) :
        Simple_ryg_logic(head, protected_sensors),
        lever_(lever), automated_lever_(nullptr)
{
}


/**
 * Aspect logic for a Red/Yellow/Green head that is controlled by a lever
 * in an interlocking tower.
 *
 * When the lever is 'normal' (inactive) the head is set to red
 * When the lever is 'reversed' (active), the head operates like a
 * normal simple ryg head until it falls to red where it will stay until
 * the lever is released and reversed again
 *
 * Some heads may have an additional 'automating' lever (often reversed
 * when the tower is closed).  If this lever is reversed along with the main
 * lever, the signal acts like a standard automated ryg head.
 */
void Interlocked_ryg_logic::loop()
{
    if(!lever_.is_indeterminate()) {
        if(lever_.is_active()) {
            // Lever is reversed

            // Run the underlying simple logic
            Simple_ryg_logic::loop();

            if(Head_aspect::red == head_.get_aspect()) {
                // If the head falls to red with the lever reversed,
                // lock it to red, unless the automated lever is also reversed

                bool is_automated = false;

                if(nullptr!=automated_lever_) {
                    if(automated_lever_->is_active()) {
                        is_automated = true;
                    }
                }

                if(false == is_automated ) {
                    head_.set_held(true);
                }
            }
        }
        else {
            // Lever is normal (inactive), clear any hold and set the aspect to red

            head_.set_held(false);

            if (Head_aspect::red != head_.get_aspect()) {
                if(head_.request_aspect(Head_aspect::red)) {
//                    Debug << F("Head ") << name_ << F(" lever normal; set to red\n");
                }
            }
        }
    }
}



/**
 * Construct the logical lever with a (presumably concrete) input lever and
 * push key sensor
 *
 * The state of the logical lever defaults to inactive, and to ensure correct
 * operation when the concrete lever is reversed, initialize the latch for
 * the concrete lever's state to inactive
 *
 * @param lever
 * @param push_key
 */
Lever_with_pushkey::Lever_with_pushkey(Sensor_interface& lever,
        Sensor_interface& push_key) :
        lever_(lever), push_key_(push_key),
        lever_reversed_(latched_false), state_(latched_false)
{
}

/// If either the lever or push key's state is currently indeterminate
/// return that the logical lever is indeterminate
bool Lever_with_pushkey::is_indeterminate() const
{
    bool return_val = false;

    if(lever_.is_indeterminate() || push_key_.is_indeterminate()) {
        return_val = true;
    }

    return return_val;
}

/**
 * Realize the logic that the logical lever will only reverse (go active) if
 * the concrete lever is reversed when the push key is already pressed
 *
 * When the state of the logical lever is requested, the latched state of
 * the lever is checked against the concrete lever's current state.  If the
 * concrete lever's state has not changed, return the latched state of the logical
 * lever.
 * If the concrete lever state has changed, check it's state (active or inactive),
 * check if the pushkey is pressed when the concrete lever goes active and then
 * latch and return appropriate states
 * @return
 */
bool Lever_with_pushkey::is_active()
{
    if(!is_indeterminate()) {
        if((bool)lever_reversed_ != lever_.is_active()) {

            if(lever_.is_active()) {
                if(!lever_reversed_) {
                    if(push_key_.is_active()){
                        // The state of the logical lever only goes true
                        // (active) if the pushkey is active when the concrete
                        // lever changes from inactive to active
                        state_ = latched_true;
                    }
                }
                // Latch that the concrete lever is reversed
                lever_reversed_ = latched_true;
            }
            else {
                state_ = latched_false;          // State is always inactive if concrete lever is normal
                lever_reversed_ = latched_false; // Latch that the concrete lever is normal
            }
        }
    }

    return (bool)state_;
}


