/*
 * loconet_txmgr.cpp
 *
 *  Created on: Dec 13, 2020
 *      Author: ackpu
 */

#include "loconet_txmgr.h"

namespace mr_signals {


// Define static constant members for external use
const Runtime_ms Loconet_txmgr::normal_tx_delay_default    = 20;       // ms
const Runtime_ms Loconet_txmgr::slow_tx_delay_default      = 200;      // ms
const Runtime_ms Loconet_txmgr::slow_tx_duration_default   = 20000;    // ms
const uint8_t    Loconet_txmgr::retransmit_limit_default   = 3;




Loconet_txmgr::Loconet_txmgr(   Runtime_ms normal_tx_delay,
                                Runtime_ms slow_tx_delay,
                                Runtime_ms slow_duration,
                                uint8_t retransmit_limit) :

                                // slow_tx_delay-1 simply makes the unit test easier to write
                                next_tx_time_(slow_tx_delay-1),retransmit_flag_(false),retransmission_count_(0),

                                normal_tx_delay_(normal_tx_delay),
                                slow_tx_delay_(slow_tx_delay),
                                slow_tx_duration_(slow_duration),
                                retransmit_limit_(retransmit_limit),
                                slow_duration_end_(0)
{

}


bool Loconet_txmgr::is_tx_allowed(const Runtime_ms current_time_ms)
{

    if(current_time_ms > next_tx_time_) {

        // For initial 'slow' period, space messages with the slow duration
        // to avoid filling the command station Loconet->DCC buffer and
        // ending up receiving LONG_ACKs
        if(current_time_ms < slow_duration_end_) {
            next_tx_time_ += slow_tx_delay_;
        }
        else {
            // Use the normal inter-message delay the rest of the time
            next_tx_time_ += normal_tx_delay_;
        }

        return true;    // Tx is allowed
    }

    return false;
}

bool Loconet_txmgr::is_retransmission()
{
    if(retransmit_flag_) {
        retransmit_flag_ = false;

        // Count and limit the number of sequential retransmission indications

        if(retransmission_count_ < retransmit_limit_) {
            retransmission_count_++;
            return true;
        }
    }

    retransmission_count_ = 0;
    return false;
}



void Loconet_txmgr::set_retransmit()
{
    retransmit_flag_ = true;

    add_tx_delay(slow_tx_delay_);
}


void Loconet_txmgr::add_tx_delay(const Runtime_ms delay)
{
    next_tx_time_ += delay;
}


void Loconet_txmgr::set_slow_duration(const Runtime_ms curr_time)
{
    slow_duration_end_ = curr_time + slow_tx_duration_;
}


} // namespace mr_signals


