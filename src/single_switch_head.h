/*
 * single_switch_head.h
 *
 *  Created on: Feb 3, 2018
 *      Author: ackpu
 */

#ifndef SRC_SINGLE_SWITCH_HEAD_H_
#define SRC_SINGLE_SWITCH_HEAD_H_


#include "head_interface.h"
#include "sensor_interface.h"
#include "switch_interface.h"

namespace mr_signals {

/**
 * Single switch output signal head
 *
 * Implements a head that controls a single output switch.
 * The switch will be set to closed for a red or dark aspect
 * The switch will be set to thrown for a yellow or green aspect
 *
 */
class Single_switch_head : public Head_interface
{
    #define single_switch_name_len 5

public:

    /// Initialize the head with a name (max length single_switch_name_len)
    /// and the output switch
    Single_switch_head(const char* name, Switch_interface& switch_1);

    Head_aspect get_aspect() override;
    bool set_aspect(const Head_aspect) override;

    const char *get_name() override;

    // TODO: Why does this return bool?
    bool loop() override;

    void set_held(const bool) override;
    bool is_held() override;


private:
    Switch_interface& switch_1_;      /// The output switch this head controls
    char name_[single_switch_name_len+1];   /// Name of the head
    Head_aspect aspect_;                    /// The head's current aspect

    char held_ : 1;                         /// The state of this head is being held
};



/**
 * Enhanced single switch signal head that requires another sensor be active
 * for the head to change to a non-red/dark aspect
 *
 * Example use is a Call-On head where the operator also needs to press a
 * pushkey for the signal to clear.
 *
 */
class Single_switch_sensor_head : public Single_switch_head
{
public:
    Single_switch_sensor_head(  const char *name,
                                Switch_interface& switch_1,
                                Sensor_interface& sensor);

    bool set_aspect(const Head_aspect aspect) override;

private:
    Sensor_interface& sensor_;
};

}


#endif /* SRC_SINGLE_SWITCH_HEAD_H_ */
