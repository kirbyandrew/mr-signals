/*
 * loconet_txmgr.h
 *
 *  Created on: Dec 13, 2020
 *      Author: ackpu
 */

#ifndef SRC_LOCONET_LOCONET_TXMGR_H_
#define SRC_LOCONET_LOCONET_TXMGR_H_

#include <stdint.h>

#include "loconet_adapter_interface.h"

namespace mr_signals {


/**
 * Implementation for a LocoNet Transmission Manager.
 *
 * This realizes the following strategy
 *
 * 1. For a given period of time on startup, messages are transmitted with a longer delay (slow) between each
 * 2. After this period the messages are transmitted at a faster (normal) rate
 * 3. If a retransmit is requested, the number of sequential retransmits is limited to avoid an infinite loop
 * 4. It a retransmit is requested, the longer delay is used for additional spacing for the next message
 *
 * Expected calling strategy:
 *
 * loop_func() {
 *    if(.is_tx_allowed()) {
 *       if(.is_retransmission(<time>)) {
 *          // Resend the last transmitted message
 *       }
 *       else {
 *          // Send the next message in the queue
 *       }
 *    }
 * }
 *
 * In response to receipt of LONG_ACK or Transmission error:
 * .set_retransmit()
 *
 *
 */

class Loconet_txmgr : public Loconet_txmgr_interface
{

public:

    static const Runtime_ms normal_tx_delay_default;    // ms
    static const Runtime_ms slow_tx_delay_default;      // ms
    static const Runtime_ms slow_tx_duration_default;   // ms
    static const uint8_t    retransmit_limit_default;

    /**
     * Constructor for the transmission manager
     *
     * All parameters are in units of ms
     *
     * @param normal_tx_delay   -   Normal inter-message delay
     * @param slow_tx_delay     -   Inter-message delay while transmitting messages slowly
     * @param slow_duration     -   Period (from startup) that the system will transmit with a slow delay
     * @param retransmit_limit  -   Maximum number of retransmissions for a single message
     */

    Loconet_txmgr(  Runtime_ms normal_tx_delay  = normal_tx_delay_default,
                    Runtime_ms slow_tx_delay    = slow_tx_delay_default,
                    Runtime_ms slow_duration    = slow_tx_duration_default,
                    uint8_t retransmit_limit    = retransmit_limit_default);

    /**
     * To be called in the transmission loop to determine whether the next
     * LocoNet message should be sent
     *
     * @return true - inter-message delay has elapsed, can transmit
     *         false - do not transmit yet
     */
    bool is_tx_allowed(const Runtime_ms curr_time) override;

    /**
     * Indicates that the last transmitted LocoNet message should be
     * resent.
     *
     * The function should only be called when attempting to send a message.
     * The retransmission strategy (e.g. number of retries) is implemented
     * using each call to this function to progress.
     *
     * @return true - retransmit last message
     *         false - do not retransmit, send next message
     */
    bool is_retransmission() override;

    /**
     * Tells the manager that the client should retransmit the last
     * transmitted LocoNet message
     *
     * is_retransmission() should return true for at least one call
     * after this is called.
     */
    void set_retransmit() override;

    /**
     * Allows a delay to be added before the next transmission (e.g. adds
     * the passed delay to the time before is_tx_allowed() will next
     * return true)
     */
    void add_tx_delay(const Runtime_ms) override;

    void set_slow_duration(const Runtime_ms curr_time) override;


protected:

    Runtime_ms next_tx_time_;           // The next time stamp to transmit at
    bool retransmit_flag_;              // Flag to indicate that a retransmission is required
    uint8_t    retransmission_count_;   // Count of sequential retransmission indications

    Runtime_ms normal_tx_delay_;        // Regular inter-message delay
    Runtime_ms slow_tx_delay_;          // Slow inter-message delay
    Runtime_ms slow_tx_duration_;       // Period (from startup) for slow transmission
    uint8_t    retransmit_limit_;       // Limit of number of retransmission indications
    Runtime_ms slow_duration_end_;
};


}



#endif /* SRC_LOCONET_LOCONET_TXMGR_H_ */
