/*
 * loconet_switch.h
 *
 *  Created on: Oct 18, 2018
 *      Author: ackpu
 */

#ifndef SRC_LOCONET_LOCONET_SWITCH_H_
#define SRC_LOCONET_LOCONET_SWITCH_H_

#include "../base/switch_interface.h"
#include "loconet_adapter_interface.h"


namespace mr_signals {


/**
 * Concrete class for switches that are controlled via LocoNet
 *
 * Inheriting from the Switch_interface, it provides an interface
 * (request_direction)that elements which uses switches (such as signal heads).
 *
 * Each instance is initialized with a Loconet Adapter which it uses to
 * transmit the switch commands on Loconet.
 *
 * LocoNet switch commands (OPC_SW_REQ) are sent with an on and off argument.
 * The command is first sent with the on argument, and then followed by the
 * same command with 'off' approximately 60ms later.  The second 'off'
 * command is sent automatically in the loop() function so that the
 * caller only uses the request_direction() API
 */
class Loconet_switch : public Switch_interface {

public:
    /**
     * Create the switch with its LocoNet Address and a reference to a valid Loconet adapter
     * @param address
     * @param ln_adapter
     */
    Loconet_switch(const Loconet_address address, Loconet_adapter_interface *ln_adapter);

    /**
     * Requests the direction of the switch be set
     * @param direction thrown or closed
     * @return true if the command was successfully enqueued, false if not
     * (caller should retry)
     */
    bool request_direction(const Switch_direction direction) override;

    /**
     * Periodic processing loop of the switch
     */
    void loop() override;

private:
    /// static global for the class; each instance refers to the same
    static Loconet_adapter_interface* ln_adapter_;

    /// Address of the switch on LocoNet
    Loconet_address address_;

    /// Time at which this switch will send the 'off' state corresponding
    /// to the on that was previously sent
    /// 0 means there is no penindg off to send
    Runtime_ms send_off_time_ms_;

    /// Current direction of the switch (used for sending
    /// the 'off' command)
    Switch_direction current_direction_;

    const Runtime_ms on_off_delay_timer_ms_=60;

};


}   // mr_signals




#endif /* SRC_LOCONET_LOCONET_SWITCH_H_ */
