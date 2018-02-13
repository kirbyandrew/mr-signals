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


class Mast_interface {
public:

    virtual void attach_head(Head_interface& head,
            Head_interface& protected_head,
            std::initializer_list<Sensor_interface *> const & protected_sensors) = 0;

    virtual void attach_head(Head_interface& head,
            std::initializer_list<Sensor_interface *> const & protected_sensors) = 0;

    virtual void loop() = 0;
//    virtual void PrintAspects() = 0;
//    virtual void debugPrint() = 0;

    virtual ~Mast_interface() = default;
};


}   // namespace mr_signals


#endif /* SRC_BASE_MAST_INTERFACE_H_ */
