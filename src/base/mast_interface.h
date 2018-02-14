/*
 * mast_interface.h
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#ifndef SRC_BASE_MAST_INTERFACE_H_
#define SRC_BASE_MAST_INTERFACE_H_

#include <vector>
#include <initializer_list>
#include "head_interface.h"
#include "sensor_interface.h"


namespace mr_signals {


class Logic_interface {
public:
    virtual void loop() = 0;

    virtual ~Logic_interface () = default;
};


}   // namespace mr_signals


#endif /* SRC_BASE_MAST_INTERFACE_H_ */
