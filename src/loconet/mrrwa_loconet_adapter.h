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

#ifdef ARDUINO

    #include "LocoNet.h"    // The MRRWA package

#else

    #include "mrrwa_loconet_mock.h"  // Assume mock instance of LocoNet needed

#endif


#define POWER_ON_DELAY_MS 200   // Declare in public API to use in unit testing



namespace mr_signals {


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
     * @param loconet       Reference to the MRWWA object in use
     * @param num_sensors   Pre-initialize the list of sensors
     */
    Mrrwa_loconet_adapter(LocoNetClass& loconet, size_t num_sensors=0);

    /**
     * Perform all setup functions required for the MRWAA Loconet class
     *
     * @param tx_pin    Arduino transmit pin
     * @param rx_pin    Arduino receive pin
     * @return          true indicates success, false that some error occured
     */
    bool setup(int tx_pin, int rx_pin) override;


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
//    void DebugPrint();

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
     * Mrrwa_loconet_adapter loconet_adpater(loconet,NUM_SENSORS);
     * ...
     * (all initialization here)
     * ...
     * if(loconet_adapter.sensor_count() > NUM_SENSORS) {
     *      // print error
     * }
     */
    size_t sensor_count();

    // TODO: Why can't this be const?
    /**
     * Get an indication of time elapsed since system startup in units of
     * milliseconds
     *
     * This function is
     *
     * @return
     */
    Runtime_ms get_time_ms();

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
     * Prints the current state of the attached sensors
     */
    void print_sensors() const;


private:

    void receive_loop();
    void transmit_loop();
    void send_global_power_on_loop();

    bool queue_loconet_msg(lnMsg *msg);

//    bool QueueLocoNetMsg(lnMsg *msg);

    /// Sensors that are notified
    /// Observer pattern; the adapter class is the subject, each sensor is an observer
    std::vector<Loconet_sensor*> sensors_;

    Runtime_ms send_gp_on_time_ms_;  // Next time from get_time_ms() to send the Global Power On message

    /// Instance of the MRWWA Loconet Class used by the adapter
    LocoNetClass& loconet_;
};

//extern MRRWALocoNetAdapter loconet;


/*
 * TODO: add timer stuff
 * constexpr std::chrono::milliseconds operator ""ms(unsigned long long ms)
{
    return chrono::milliseconds(ms);
}
constexpr std::chrono::duration<long double, std::milli> operator ""ms(long double ms)
{
    return std::chrono::duration<long double, std::milli>(ms);
}
 */

}

void set_mrrwa_loconet_adapter(mr_signals::Mrrwa_loconet_adapter * const adapter);

#endif /* SRC_LOCONET_MRRWA_LOCONET_ADAPTER_H_ */
