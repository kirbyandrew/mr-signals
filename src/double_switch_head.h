/*
 * double_switch_head.h
 *
 *  Created on: Feb 6, 2018
 *      Author: ackpu
 */

#ifndef SRC_DOUBLE_SWITCH_HEAD_H_
#define SRC_DOUBLE_SWITCH_HEAD_H_

#include "base/head_interface.h"
#include "sensor_interface.h"
#include "base/switch_interface.h"

namespace mr_signals {


/**
 * Double switch output signal head
 *
 * Implement a head that controls two switch outputs.  The switches are set
 * as follows:
 *
 * Aspect   Switch1     Switch2
 * dark     closed      c
 * green    thrown      c
 * yellow   t           t
 * red      c           t
 */

class Double_switch_head : public Head_interface
{

public:

    /// Initialize the head with a name and the two switches it controls
    Double_switch_head(const char *name, Switch_interface& switch_1,
            Switch_interface& switch_2);

    void loop() override;

protected:
    /// Specialized output functionality
    bool request_outputs(const Head_aspect) override;


    Switch_interface& switch_1_;
    Switch_interface& switch_2_;
};


} // namespace mr_signals

#endif /* SRC_DOUBLE_SWITCH_HEAD_H_ */
