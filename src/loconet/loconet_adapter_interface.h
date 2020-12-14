/*
 * loconet_adapter_interface.h
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */

#ifndef SRC_LOCONET_LOCONET_ADAPTER_INTERFACE_H_
#define SRC_LOCONET_LOCONET_ADAPTER_INTERFACE_H_

#include <stdint.h>

namespace mr_signals {

class Loconet_sensor;   // Forward declaration for use in Loconet_adapter_interface


typedef uint16_t Loconet_address;
typedef uint32_t Runtime_ms;

/**
 *  Interface class that defines all of the methods necessary for a
 *  Loconet_adapter_interface to interact with a concrete instance
 *  of a transmission manager (txmgr).
 *
 *  The primary purpose of the transmission manager is to encapsulate
 *  the functionality required to
 *
 *  1. Provide a delay between transmitting messages on LocoNet to reduce flooding
 *  2. Provide a different delay on startup (to allow all messages to be sent
 *     without triggering a LONG_ACK by filling up the command station
 *     LocoNet->DCC buffer) to regular operation
 *  3. Provide an indication that a message should be retried (due to either
 *     a transmission error or the asynchronous receipt of a LONG_ACK)
 *  4. Increase the inter-message delay in the event of a retry to reduce
 *     the probability of encountering another LONG_ACK
 *
 */

class Loconet_txmgr_interface
{

public:
    /**
     * To be called in the transmission loop to determine whether the next
     * LocoNet message should be sent
     *
     * @return true - inter-message delay has elapsed, can transmit
     *         false - do not transmit yet
     */
    virtual bool is_tx_allowed() = 0;

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
    virtual bool is_retransmission() = 0;

    /**
     * Tells the manager that the client should retransmit the last
     * transmitted LocoNet message
     *
     * is_retransmission() should return true for at least one call
     * after this is called.
     */
    virtual void set_retransmit() = 0;

    /**
     * Allows a delay to be added before the next transmission (e.g. adds
     * the passed delay to the time before is_tx_allowed() will next
     * return true)
     */
    virtual void add_tx_delay(Runtime_ms) = 0;
};


/**
 *  Interface class that defines all of the methods neccessary for mr_signals to
 *  interact with a concrete instance of a object that communicates with
 *  Digitrax's LocoNet bus
 *
 *  This interface should be inherited from to use a package that realizes
 *  the Loconet communication
 */

//TODO: Add command to slowly go through and send out the state of all switches!
class Loconet_adapter_interface
{
public:


    /**
     * Provides an interface to attach a loconet sensors to the loconet
     * adapter so that the adapter can set their state as bus traffic
     * is received and processed within the loop
     * @param
     */
    virtual void attach_sensor(Loconet_sensor *) = 0;


    /**
     * Allow other objects to send the OpcSwReq (switch request) Loconet message
     * @param address The address to send the switch command
     * @param thrown thrown (true) or closed (false) state to send
     * @param on on/off argument for OpcSwReq
     * @return true if the command was successfully sent / queued
     *          false if the command was not sent and should be retried if needed
     */
    virtual bool send_opc_sw_req(Loconet_address address, bool thrown, bool on) = 0;

    /**
     * Allow other objects to send the Global Power On message (typically to
     * trigger reporting of sensor states by Loconet devices
     * @return true if the command was successfully sent / queued
     *          false if the command was not sent and should be retried if needed
     */
    virtual bool send_opc_gp_on()= 0;


    /**
     * Allows the client to insert a delay in the LocoNet transmission stream
     * to avoid filling the Command Station's Loconet -> DCC buffer.
     * @param Delay in ms
     * @return true if the tx delay was successfully queued
     */

    virtual bool insert_ln_tx_delay(uint8_t) = 0;

    virtual ~Loconet_adapter_interface() = default;


    /**
     * Provides a consistent time base for elements to use
     * @return
     */
    virtual Runtime_ms get_time_ms() const = 0;

};



}

#endif /* SRC_LOCONET_LOCONET_ADAPTER_INTERFACE_H_ */
