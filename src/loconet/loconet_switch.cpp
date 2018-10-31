/*
 * loconet_switch.cpp
 *
 *  Created on: Oct 18, 2018
 *      Author: ackpu
 */

#include "loconet_switch.h"

namespace mr_signals {

// static instance of the LocoNet adapter reference
Loconet_adapter_interface* Loconet_switch::ln_adapter_ = nullptr;


Loconet_switch::Loconet_switch(const Loconet_address address, Loconet_adapter_interface *ln_adapter) :
        address_(address), send_off_time_ms_(0), current_direction_(Switch_direction::unknown)
{
    // Don't bother with protecting against NULL; if an invalid argument is passed
    // the system will just crash
    ln_adapter_ = ln_adapter;
}

bool Loconet_switch::request_direction(const Switch_direction direction) {


    // Store switch direction
    current_direction_ = direction;

    // Send switch request with argument 'on'
    bool result = ln_adapter_ -> send_opc_sw_req(   address_,
                                                    Switch_direction::thrown == current_direction_ ? true : false,
                                                    true);

    if(result) {
        // If the 'on' command is successfully stored, determine when to request the 'off' command
        send_off_time_ms_ = ln_adapter_->get_time_ms() + on_off_delay_timer_ms_;
    }

    return(result);
}

/**
 * Periodic processing loop of the switch
 */
void Loconet_switch::loop() {

    if(send_off_time_ms_) {

        if(ln_adapter_->get_time_ms() >= send_off_time_ms_) {

            // Time to send, check that our switch direction makes sense
            if( (Switch_direction::closed == current_direction_) ||
                (Switch_direction::thrown == current_direction_))
            {
                // Ignore the return code; if this fails, just give up and leave the physical
                // switch in an indeterminate state
                (void)  ln_adapter_ -> send_opc_sw_req( address_,
                                                        Switch_direction::thrown == current_direction_ ? true : false,
                                                        false);
            }

            // Clear flag
            send_off_time_ms_ = 0;
        }
    }
}



} // namespace mr_signals
