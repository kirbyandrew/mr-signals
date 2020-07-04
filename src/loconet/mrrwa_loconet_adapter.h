/*
 * mrwwa_loconet_adapter.h
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */

#ifndef SRC_LOCONET_MRRWA_LOCONET_ADAPTER_H_
#define SRC_LOCONET_MRRWA_LOCONET_ADAPTER_H_

#include <vector>
#include "loconet_adapter_interface.h"
#include "loconet_sensor.h"
#include "../base/circular_buffer.h"


#ifdef ARDUINO

    #include "LocoNet.h"    // The MRRWA package

#else

    #include "mrrwa_loconet_mock.h"  // Assume mock instance of LocoNet needed

#endif


#define POWER_ON_DELAY_MS 200   // Declare in public API to use in unit testing



namespace mr_signals {




/**
 * Handles the management of the LocoNet Transmit buffer for Mrrwa_loconet_adapter
 *
 * This class is private to Mrrwa_loconet_adapter and should not be accessed
 * directly at any time.
 *
 */
class Mrrwa_loconet_tx_buffer
{
public:

    bool initialize(std::size_t buffer_size);


    /**
     * Queue a lnMsg (Loconet message) for transmission onto LocoNet
     *
     * @param msg - The Loconet message to queue
     * @return true if enqueued, false otherwise
     */
    bool queue_loconet_msg(lnMsg& msg);

    /**
     *  Dequeue a queued lnMsg (Loconet message)
     *
     * @param msg - The dequeued message
     * @return true if a message was dequed, false if not
     */
    bool dequeue_loconet_msg(lnMsg& msg);


    Circular_buffer loconet_tx_buffer_;
};



/**
 * Adapts the MRRWA Loconet library to the mr-signals interfaces
 * and conventions, integrates Loconet Sensor objects and provides tge
 * interface for LocoNet switch objects to use.
 *

 * The adapter provides an observer pattern subject and maintains a collection
 * of Loconet Sensor objects which observe the subject.  The adapter converts
 * received LocoNet sensor messages into sensor states.
 *
 */

class Mrrwa_loconet_adapter : public Loconet_adapter_interface
{
public:


    /**
     * Construct an instance that holds a reference to an MRRWA LocoNetClass
     * with an option to pre-initialize the number of sensors that it holds
     * to more efficiently use RAM
     *
     * Example
     * #include "Loconet.h" // mrrwa loconet
     *
     * LoconetClass loconet;
     *
     * Mrrwa_loconet_adapter loconet_adpater(loconet,50);
     *
     * @param loconet           Reference to the MRWWA object in use
     * @param tx_pin            Arduino transmit pin
     * @param num_sensors       Pre-initialize the list of sensors
     * @param tx_buffer_size    Set the transmit buffer queue size.  IMPORTANT!  This must
     *                          be large enough to hold all messages that may be transmitted simultaneously
     *                          otherwise the system could enter an unrecoverable state.
     *                          For each double output head, assume 4x3-byte messages
     *                          A value of many hundreds is recommended.  The buffer high watermark
     *                          can be accessed by get_buffer_high_watermark and printed periodically.
     */
    Mrrwa_loconet_adapter(LocoNetClass& loconet, int tx_pin=2, size_t num_sensors=0, size_t tx_buffer_size=100);


    /**
     * Call in Arduino sketch setup() function
     */

    void setup();


    /**
     * Processing loop; to be called as often as possible to process incoming
     * messages and transmit outgoing messages
     */
    void loop() override;


    /**
     * Attach a sensor to the adapter so that the adapter can inform the sensor
     * when a sensor state message is received (active or inactive)
     *
     * This is called automatically by the Loconet_sensor constructor and
     * so normally is not called directly.  It is provided for alternative
     * implementations to use if desired.
     *
     * @param sensor  Sensor which wishes to observe the adapter
     */
    void attach_sensor(Loconet_sensor* sensor) override;

    /**
     * Requests that a Switch Request Loconet message be queued
     *
     * @param address   Address of the switch
     * @param thrown    true = thrown, false = closed
     * @param on        on/off of the Loconet protocol
     * @return          true if the request was queued, false if not
     */
    bool send_opc_sw_req(Loconet_address address, bool thrown,bool on) override;

    /**
     * Requests that a General Power On LocoNet message be sent
     * @return  true if the message was sent, false if not
     */
    bool send_opc_gp_on() override;


    /**
     * Get the number of sensors attached to the adapter
     *
     * Primary use is to check that the number of sensors does not exceed
     * the number that the adapter was initialized to hold (the adapter
     * will hold more sensors, however it is inefficient as the buffer will
     * continue to be re-allocated)
     *
     * @return Number of sensors
     *
     *
     * Example...
     * #include "Loconet.h" // mrrwa loconet
     *
     * LoconetClass loconet;
     *
     * #define NUM_SENSORS 50
     *
     * Mrrwa_loconet_adapter loconet_adpater(loconet,<tx_pin>,NUM_SENSORS);
     * ...
     * (all initialization here)
     * ...
     * if(loconet_adapter.sensor_count() > NUM_SENSORS) {
     *      // print error
     * }
     */
    size_t sensor_count();

    size_t sensor_init_size();

    bool any_sensor_indeterminate();

     /**
     * Get an indication of time elapsed since system startup in units of
     * milliseconds
     *
     * This function is
     *
     * @return
     */
     Runtime_ms get_time_ms() const override;

//    void queue_loconet_msg(lnMsg *msg);

    //void PrintSensors();

    /**
     * Triggers the notification of the sensors attached to the adapter
     * of the change of state of a sensor.
     *
     * Primary use is to be called when a sensor message is received from
     * LocoNet
     *
     * Function is const as it does not affect the contents of the adapter
     * object (only the attached sensor objects).  This also allows it to be
     * called from a constant pointer in notifySensor(), e.g.
     *
     * const mr_signals::Mrrwa_loconet_adapter* loconet_adapter;
     *
     * @param address   Address of the sensor from LocoNet
     * @param state     State of the sensor (true=active, false=inactive)
     */
    void notify_sensors(uint16_t address, bool state) const;


    /**
     * Retrieve the transmit buffer high water mark
     * @return The maximum occupancy of the transmit buffer
     */
    std::size_t get_buffer_high_watermark() {
        return tx_buffer_.loconet_tx_buffer_.high_watermark();
    }

    /**
     * Retrieve the internal transmit error count
     * @return The tx error count
     */
    uint16_t get_tx_error_count() {
        return tx_errors_;
    }

    /**
     * Retrieve the internal count of OPC_LONG_ACKs (error from command station in response to switch request) received
     * @return The long ack count
     */
    uint16_t get_long_ack_count() {
        return long_acks_;
    }


    /**
     * Prints the current state of the attached sensors using
     * the Serial stream from mr_signals.h
     */
    void print_sensors() const;


    /**
     *  Prints the content of an lnMsg in 2 byte hex format with a prefix
     *  (e.g. "LN RX" or "LN TX") using the Serial output stream from mr_signals.h
     *  A timestamp in milliseconds is prepended to the trace.
     *
     * @param packet - Pointer to a lnMsg to print
     * @param prefix - String prefix to print
     * @param print_checksum - Indicates whether or not to print the last
     *                 byte in the lnMsg, which is the checksum.  Pass false
     *                 if this function is being used where the checksum byte
     *                 is not initialized and will otherwise cause confusion
     *                 if printed.
     *
     */

    void print_lnMsg(lnMsg *packet, const char *prefix, bool print_checksum);


private:

    void receive_loop();
    void transmit_loop();
    void send_global_power_on_loop();



    /// Sensors that are notified
    /// Observer pattern; the adapter class is the subject, each sensor is an observer
    std::vector<Loconet_sensor*> sensors_;

    size_t sensor_init_size_;       // The size the sensor vector is initialized to (to compare against its final size)


    Runtime_ms next_tx_time_ms_;    // Next time from get_time_ms() that the
                                    // adapter will attempt to transmit a
                                    // queued message


    Runtime_ms send_gp_on_time_ms_;  // Next time from get_time_ms() to send the Global Power On message

    static const Runtime_ms transmit_delay_ms_ = 10;

    Mrrwa_loconet_tx_buffer tx_buffer_;

    /// Count of transmit errors from the MRRWA library
    uint16_t tx_errors_;

    // Count of LONG_ACKs received for switch messages
    uint16_t long_acks_;

    /// Instance of the MRWWA Loconet Class used by the adapter
    LocoNetClass& loconet_;

    /// Pin used to transmit
    int tx_pin_;

    bool any_sensor_indeterminate_;

};

}


#endif /* SRC_LOCONET_MRRWA_LOCONET_ADAPTER_H_ */
