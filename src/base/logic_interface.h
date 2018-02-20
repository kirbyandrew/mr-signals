/*
 * mast_interface.h
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#ifndef SRC_BASE_LOGIC_INTERFACE_H_
#define SRC_BASE_LOGIC_INTERFACE_H_

#include <vector>
#include <initializer_list>
#include "head_interface.h"
#include "sensor_interface.h"


namespace mr_signals {


/**
 * Abstract interface for the logic that controls a head or heads
 *
 * Each concrete implementation is analogous to the relay control logic of a
 * traditional signal installation.  It integrates all other objects
 * that are involved in determining the aspect of a head, and realizes
 * the corresponding logic.
 *
 */
class Logic_interface {
public:
    virtual void loop() = 0;

    virtual ~Logic_interface () = default;
};


}   // namespace mr_signals


#endif /* SRC_BASE_LOGIC_INTERFACE_H_ */
