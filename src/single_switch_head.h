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

    /// Specialized output functionality for the QuadLN_S
    bool request_outputs(const Head_aspect) override;

    void loop() override;

    // TODO: How to find all classes that don't have virtual destructors??  Why needed?

private:
    Switch_interface& switch_1_;
};



/**
 * Enhanced single switch signal head that requires another sensor be active
 * for the head to change to a non-red/dark aspect
 *
 * Example use is a Call-On head where the operator also needs to press a
 * pushkey for the signal to clear.
 *
 */
/*
class Single_switch_sensor_head : public Single_switch_head
{
public:
    Single_switch_sensor_head(  const char *name,
                                Switch_interface& switch_1,
                                Sensor_interface& sensor);

    bool request_aspect(const Head_aspect aspect) override;

  //  void loop() override;

private:
    Sensor_interface& sensor_;
};
*/
}


#endif /* SRC_SINGLE_SWITCH_HEAD_H_ */
