/*
 * single_switch_head.h
 *
 *  Created on: Feb 3, 2018
 *      Author: ackpu
 */

#ifndef SRC_SINGLE_SWITCH_HEAD_H_
#define SRC_SINGLE_SWITCH_HEAD_H_


#include "base/head_interface.h"
#include "sensor_interface.h"
#include "base/switch_interface.h"

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
public:

    /// Initialize the head with a name and the output switch
    Single_switch_head(const char* name, Switch_interface& switch_1);

    void loop() override;

protected:
    /// Specialized output functionality
    bool request_outputs(const Head_aspect) override;

    Switch_interface& switch_1_;
};


}


#endif /* SRC_SINGLE_SWITCH_HEAD_H_ */
