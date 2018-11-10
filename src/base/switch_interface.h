/*
 * switch_interface.h
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */

#ifndef SWITCH_INTERFACE_H_
#define SWITCH_INTERFACE_H_

#include <stdint.h>

namespace mr_signals {

/// Bus (DCC, loconet etc) switch states
enum class Switch_direction : uint8_t {
    closed,      /// The switch is in the closed position
    thrown,      /// The switch is in the open position
    unknown      /// The state of the switch is unknown
};

/**
 * Abstract interface to address DCC (or similar) switches to be used
 * by classes that manipulate the state of physical switches.
 *
 * The state of a switch can be requested to be thrown or closed.
 * A switch state is requested rather than set to allow for the
 * detection of an error (and retry of) in setting a physical switch's state
 *
 * Concrete implementations that control physical switches are
 * recommended to initialize the switch state to
 * switch_unknown, both to make unit testing easier and to
 * simplify the logic to determine if a state change is being requested
 * before sending out a command on the bus.
 *
 */
class Switch_interface {
public:
    virtual bool request_direction(const Switch_direction)=0;
    virtual void loop()=0;
    virtual ~Switch_interface() = default;

};


/// This class is used to support the unit tests
class Test_switch : public Switch_interface {
public:


    Test_switch(int num = -1) :
        direction_(Switch_direction::unknown), num_(num), loop_cnt_(0), lock_(false)
    {
    }

    /// In this test implementation, the request to change direction is always
    /// successful.  Tests for
    bool request_direction(const Switch_direction direction) override
    {

        if(lock_) {
            return false;
        }
        else {
            direction_ = direction;

            return true;
        }
    }

    void loop() override {
        loop_cnt_++;
    }

    /// Let tests access the switch's direction
    Switch_direction  get_direction() const { return direction_; }

    /// Let tests access the number of times .loop() has been called
    int get_loop_cnt() const { return loop_cnt_; }

    /// Locks a switch so that requests to change the direction fail
    void set_lock(const bool lock) { lock_ = lock; }

protected:

    Switch_direction direction_;    /// Direction of the switch
    int num_;                       /// Switch number for test convenience
    int loop_cnt_;
    bool lock_;
};

}



#endif /* SWITCH_INTERFACE_H_ */
