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


typedef uint16_t Loconet_address;   // TODO: Right format for new type?
typedef uint32_t Runtime_ms;       // TODO: Surely a better things for this?

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
    // Allows passing the Arduino pins that are used for sending and receiving
    virtual bool setup(int tx_pin, int rx_pin) = 0;

    // General processing loop to be called within the main loop() function
    virtual void loop() = 0;

    // Attach all loconet sensors to the loconet adapater so that the
    // adapter can set their state as bus traffic is received and processed
    // within the loop
    virtual void attach_sensor(Loconet_sensor *) = 0;


    /**
     * Allow other objects to send the OpcSwReq (switch request) Loconet message
     * @param address The address to send the switch command
     * @param thrown thrown (true) or closed (false) state to send
     * @param on on/off argument for OpcSwReq
     * @return true if the command was successfully sent / queued
     *          false if the command was not sent and should be retried if needed
     */virtual bool send_opc_sw_req(Loconet_address address, bool thrown, bool on) = 0;

    /**
     * Allow other objects to send the Global Power On message (typically to
     * trigger reporting of sensor states by Loconet devices
     * @return true if the command was successfully sent / queued
     *          false if the command was not sent and should be retried if needed
     */virtual bool send_opc_gp_on()= 0;

    virtual ~Loconet_adapter_interface() = default;


protected:
    virtual Runtime_ms get_time_ms() = 0;

};

}

#endif /* SRC_LOCONET_LOCONET_ADAPTER_INTERFACE_H_ */
