/*
 * masts.cpp
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#include "algorithm.h"
#include "masts.h"

using namespace mr_signals;

// TODO: Where should the logic to control the heads go? Since the mast implements
// the logic, can it be adapted for different types of output?
// How would you add a 3 color and a 5 color head together?
// If you specialize a different mast, can you reuse the same interlocking?

// Ideally pass an algorithm with each head, but that's then another reference per head


// TODO: If a mast doesn't have a interlocking lever, there's really no need to
// associated them together in a mast.  Really the mast is just implementing the
// logic for the head; they can be collected together and processed like the masts

// Try breaking them up after getting them working :) to compare memory consumption


// To test in Arduino
// - cost of passing initializer list
// comparison of individual classes for 'head_logic' rather than masts that collect it

Standard_mast::Standard_mast() {}

// Attach a head and protected sensor list only
void Standard_mast::attach_head(Head_interface& head,
            std::initializer_list<Sensor_interface *> const & protected_sensors)
{
    heads_.push_back(Mast_head { head, nullptr, protected_sensors });
}

// Attach a head and protected sensor list only
void Standard_mast::attach_head(Head_interface& head,
        Head_interface& protected_head,
        std::initializer_list<Sensor_interface *> const & protected_sensors)
{
    heads_.push_back(Mast_head { head, &protected_head, protected_sensors });
}


/**
 * Aspect logic for a 3 aspect head that protects sensors and may protect
 * another head
 *
 * If any protected sensor is active, set the aspect to red
 * If no protected sensors are active, but the protected head is red, set the
 * aspect to yellow
 * If no protected sensor is active, and the protected head is not red, set the aspect to green
 *
 * If the state of any protected sensor is unknown, delay processing until the
 * state is known to avoid potential race conditions
 *
 * @param head
 * @return
 */
bool Standard_mast::process_head(Mast_head& head)
{
    bool changed = false;

    head.head.loop();

    // Check that the state of all protected sensors are known
    if (std::none_of(head.protected_sensors_.begin(),
            head.protected_sensors_.end(),
            [](Sensor_interface* sensor) {return sensor->is_indeterminate();})) {

        // No sensors are indeterminate, process the head's setting
        Head_aspect aspect = unknown;

        // First check the protected sensors
        if (std::any_of(head.protected_sensors_.begin(),
                head.protected_sensors_.end(),
                [](Sensor_interface* sensor) {return sensor->get_state();})) {

            // A protected block is occupied, set the head red
            aspect = red;
        }
        else {
            // Since the protected blocks are clear, check the protected head
            // and determine a green or yellow aspect

            bool protectedHeadStop = false;

             if (nullptr != head.protected_head) {
                 if (red == head.protected_head->get_aspect()) {
                     protectedHeadStop = true;
                 }
             }

            if (protectedHeadStop) {
                // Protected signal is at stop, so set this head to Caution/yellow
                aspect = yellow;
            } else {
                // Both the protected blocks are clear and the protected signal is
                // not at stop (or doesn't exist); set a Proceed indication
                aspect = green;
            }
        }

        // If the new aspect determined for this head differs from its current
        // value, request that the head change
        if (aspect != head.head.get_aspect()) {
            if ((changed = head.head.request_aspect(aspect)) == true) {
//                Debug << F("Mast ") << name_.c_str() << head.head->GetName() << F(" changed to ") << GetAspectString(head.head -> GetAspect()) << F("\n");
            }
        }
    }

    return changed;

}


void Standard_mast::loop()
{
    // Evaluate each head attached to this mast
    for (Mast_head& head : heads_) {
        process_head(head);
    }

}
