/*
 * pin_switch.h
 *
 *  Created on: Nov 18, 2018
 *      Author: ackpu
 */

#ifndef SRC_PIN_SWITCH_H_
#define SRC_PIN_SWITCH_H_

#ifndef ARDUINO
#include "arduino_mock.h"   // pinMode(), digitalWrite() for unit tests not on Arduino
#else
#include "Arduino.h"
#endif

#include "./base/switch_interface.h"
#include "setup_funcs.h"

namespace mr_signals {


class Pin_switch : public Switch_interface, Setup_interface {

public:
    /**
     * Create the switch with its LocoNet Address and a reference to a valid Loconet adapter
     * @param address
     * @param ln_adapter
     */
    Pin_switch(Setup_collection &setup_collection, const uint8_t pin) : Setup_interface(setup_collection), pin_(pin) {
    }


    void setup() override {
        pinMode(pin_,OUTPUT);

        request_direction(Switch_direction::closed);
    }

    void loop() override {}


    bool request_direction(const Switch_direction direction) override {
        if(Switch_direction::thrown == direction) {
            digitalWrite(pin_,HIGH);
        }
        else {
            digitalWrite(pin_,LOW);
        }

        return true;
    }

protected:
    uint8_t pin_;
};


}

#endif /* SRC_PIN_SWITCH_H_ */


