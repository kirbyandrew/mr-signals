/*
 * switch_interface.h
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */

#ifndef SWITCH_INTERFACE_H_
#define SWITCH_INTERFACE_H_


namespace mr_signals {

/// Bus (DCC, loconet etc) switch states
enum Switch_direction {
    switch_closed,      /// The switch is in the closed position
    switch_thrown,      /// The switch is in the open position
    switch_unknown      /// The state of the switch is unknown
};

class Switch_interface {
public:
    virtual bool request_direction(const Switch_direction)=0;
    virtual void loop()=0;
    virtual ~Switch_interface() = default;

};


/// This class is used to support the unit tests
class Test_switch : public Switch_interface {
public:


    Test_switch(int num=-1) : num_(num), loop_cnt_(0) {
        direction_ = switch_unknown;
    }

    /// In this test implementation, the request to change direction is always
    /// successful.  Tests for
    bool request_direction(const Switch_direction direction) override {

        direction_ = direction;

        return true;
    }

    void loop() override {
        loop_cnt_++;
    }

    /// Let tests access the switch's direction
    Switch_direction  get_direction() { return direction_; }

    /// Let tests access the number of times .loop() has been called
    int get_loop_cnt() { return loop_cnt_; }

protected:

    Switch_direction direction_;    /// Direction of the switch
    int num_;                       /// Switch number for test convenience
    int loop_cnt_;
};

}



#endif /* SWITCH_INTERFACE_H_ */
