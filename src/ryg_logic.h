/*
 * masts.h
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#ifndef SRC_RYG_LOGIC_H_
#define SRC_RYG_LOGIC_H_

#include <stdint.h>
#include <vector>
#include <initializer_list>
#include "base/logic_interface.h"
#include "head_interface.h"
#include "sensor_interface.h"



namespace mr_signals {

class Simple_ryg_logic : public Logic_interface
{
public:
    Simple_ryg_logic(Head_interface& head,
            Head_interface& protected_head,
            std::initializer_list<Sensor_interface *> const & protected_sensors);

    Simple_ryg_logic(Head_interface& head,
            std::initializer_list<Sensor_interface *> const & protected_sensors);

    void loop() override;

protected:

    Head_interface& head_;               // Reference as there must be a head
    Head_interface* protected_head_;     // Pointer as there may not be a protected head. nullptr used when not present
    std::vector<Sensor_interface *> protected_sensors_;
};

class Interlocked_ryg_logic : public Simple_ryg_logic
{
public:
    // Head, protected head, interlocking lever and protected sensors
    // No automated lever
    Interlocked_ryg_logic(Head_interface& head,
            Head_interface& protected_head,
            Sensor_interface& lever,
            std::initializer_list<Sensor_interface *> const & protected_sensors);

    // Head, protected head, interlocking lever, automated lever and
    // protected sensors
    Interlocked_ryg_logic(Head_interface& head,
            Head_interface& protected_head,
            Sensor_interface& lever,
            Sensor_interface& automated_lever,
            std::initializer_list<Sensor_interface *> const & protected_sensors);

    // Head, interlocking lever and protected sensors
    // No protected head or automated lever
    Interlocked_ryg_logic(Head_interface& head,
            Sensor_interface& lever,
            std::initializer_list<Sensor_interface *> const & protected_sensors);

    void loop() override;

private:
    Sensor_interface& lever_;               // Reference as mandatory
    Sensor_interface* automated_lever_;     // Pointer as optional
};


/**
 * Implements the logic of a interlocking lever that has a dependency on
 * a push-key to fully reverse. In an actual interlocking frame, such a lever
 * had a normally closed mechanical lock that held it either at normal (inactive)
 * or in its current position (normal or reversed)
 *
 * Pushing the push key energized a normally open circuit that would enforce
 * some preconditions before allowing the lever to be reversed (normally
 * that certain track circuits were clear).
 *
 * This was common for clearing a low-speed (or call-on) signals or before
 * allowing switches and FSLs to be thrown.
 *
 * In a model railway implementation, the push-key can range from a simple
 * physical button for an operator to press, or be realized as a much more
 * complex concrete sensor if desired.
 *
 */
class Lever_with_pushkey : public Sensor_interface
{
public:
    Lever_with_pushkey(Sensor_interface& lever, Sensor_interface& push_key);

    bool get_state() override;
    bool is_indeterminate() override;

private:
    Sensor_interface& lever_;
    Sensor_interface& push_key_;

    enum : uint8_t {
        latched_false = 0,
        latched_true = 1

    };

    uint8_t lever_reversed_ : 1;
    uint8_t state_ : 1;
};

}


#endif // SRC_RYG_LOGIC_H_
