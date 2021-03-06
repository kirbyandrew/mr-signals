/*
 * quad_ln_head.h
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#ifndef SRC_QUADLN_S_HEAD_H_
#define SRC_QUADLN_S_HEAD_H_

#include "base/head_interface.h"
#include "sensor_interface.h"
#include "base/switch_interface.h"
#include "double_switch_head.h"

namespace mr_signals {


/**
 * Implements a head that controls a Tam Valley QuadLN_S driver
 *
 * The QuadLN_S uses two switches, but differs from the Double Head switch
 * head in how the switches are controlled; specifically one switch (switch_1_,
 * inherited from Double_switch_head) controls the two end positions
 * (Green and Red for a servo semaphore), with the second (switch_2_)
 * (Midpoint in QuadLN documentation) overriding the other with a center position
 * (yellow) when thrown
 *
 *
 * t = thrown
 * c = closed
 *
 * Aspect   Switch1     Switch2(Midpoint)   Comment
 * dark     -           -                   Not supported, will always fail
 * green    t           c
 * yellow   -           t
 * red      c           c
 */
class Quadln_s_head : public Double_switch_head
{

public:

    /// Requires a name for the head and the associated switch and midpoint for the QuadLN_S
    Quadln_s_head(const char *name, Switch_interface& switch_1,
            Switch_interface& midpoint_switch);

protected:
    /// Specialized output functionality for the QuadLN_S
    bool request_outputs(const Head_aspect) override;

};




} // namespace mr_signals




#endif /* SRC_QUADLN_HEAD_H_ */
