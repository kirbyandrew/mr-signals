/*
 * triple_digital_head.h
 *
 *  Created on: Nov 18, 2018
 *      Author: ackpu
 */

#ifndef SRC_LOCONET_TRIPLE_DIGITAL_HEAD_H_
#define SRC_LOCONET_TRIPLE_DIGITAL_HEAD_H_

#include "base/head_interface.h"

namespace mr_signals {


/**
 * Triple digital pin output signal head.
 *
 * Controls 3 digital I/O pins to show red, yellow, red and dark aspects.
 *
 */

class Triple_pin_head: public Head_interface
{

public:

    /// Initialize the head with a name and the three pins that it controls
    Triple_pin_head(const char *name, uint8_t green_pin, uint8_t yellow_pin, uint8_t red_pin);

    bool request_aspect(const Head_aspect) override;

    // Does nothing
    void loop() override {}

protected:

    uint8_t green_pin_;
    uint8_t yellow_pin_;
    uint8_t red_pin_;
};


} // namespace mr_signals




#endif /* SRC_LOCONET_TRIPLE_DIGITAL_HEAD_H_ */
