/*
 * mrrwa_loconet_adapter.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */

#include "mrrwa_loconet_adapter.h"

#include "mr_signals.h"

#include <algorithm>

#ifndef ARDUINO
#include "arduino_mock.h"   // millis()
#endif



/*
 *
 * Global required to get a local instance of the adapter so that
 * adapter functions can be called from stand-alone C functions that the
 * MRRWA library calls directly such as notifySensor()
 *
 * This is all declared outside of the usual mr_signals scope so that the linker
 * finds it for the MRRWA library
 */

static const mr_signals::Mrrwa_loconet_adapter* loconet_adapter=nullptr;

void set_mrrwa_loconet_adapter(mr_signals::Mrrwa_loconet_adapter * const adapter)
{
    loconet_adapter = adapter;
}




// Callback for the MRRWA LocoNet module (called from LocoNet.processSwitchSensorMessage)
// for all Sensor messages
void notifySensor(uint16_t Address, uint8_t State)
{

    Serial << F("Sensor Msg: ") << std::dec << Address << F(" - ") << (State ? F("Active") : F("Inactive")) << F("\n");

    if(nullptr!= loconet_adapter) {
        loconet_adapter->notify_sensors((mr_signals::Loconet_address)Address, State ? true : false);
    }
}




namespace mr_signals {


Mrrwa_loconet_adapter::Mrrwa_loconet_adapter(LocoNetClass& loconet,size_t num_sensors) :
        send_gp_on_time_ms_(0), loconet_(loconet)
{
    if(num_sensors) {
        sensors_.reserve(num_sensors);
    }
}


bool Mrrwa_loconet_adapter::setup(int tx_pin, int rx_pin)
{
    if (rx_pin) {}          // Not used in MRRWA LocoNet
    loconet_.init(tx_pin);


    send_gp_on_time_ms_ = get_time_ms() + POWER_ON_DELAY_MS;

    //     init_queue(&ln_tx_queue);


    return true;    // No return from MRRWA Loconet .init function, always return true here
}


void Mrrwa_loconet_adapter::loop()
{
    receive_loop();

//    transmit_loop();

    send_global_power_on_loop();
}


void Mrrwa_loconet_adapter::attach_sensor(Loconet_sensor* sensor)
{
    sensors_.push_back(sensor);
}


bool Mrrwa_loconet_adapter::send_opc_sw_req(Loconet_address address, bool thrown,bool on)
{
    return true;
}


bool Mrrwa_loconet_adapter::send_opc_gp_on()
{
    return LN_DONE==loconet_.reportPower(1) ? true : false;
}


Runtime_ms Mrrwa_loconet_adapter::get_time_ms()
{
    return millis();
}

size_t Mrrwa_loconet_adapter::sensor_count()
{
    return sensors_.size();
}




//    void queue_loconet_msg(lnMsg *msg);

    //void PrintSensors();

void Mrrwa_loconet_adapter::notify_sensors(Loconet_address address, bool state) const
{

    std::find_if(sensors_.begin(),
        sensors_.end(),
        [address, state](Loconet_sensor * sensor) {

        if (sensor->notify(address, state)) {
            Serial << F("Set Sensor ") << sensor->get_name() << " -> " << (state ? F("Active\n") : F("Inactive\n"));

            return true;    // Found the sensor, stop the find_if() loop
        }
        else {
            return false;   // Continue the find_if() loop
        }
    });
}



void Mrrwa_loconet_adapter::receive_loop()
{
    lnMsg        *ln_packet;

    ln_packet = loconet_.receive();
//    if (nullptr != ln_packet) {

// TODO : Test this on both an Arduino and a PC!
    if(ln_packet) {

        Serial << F("LN RX : ");



        uint8_t msg_len = getLnMsgSize(ln_packet);

        for (uint8_t x = 0; x < msg_len; x++)
        {
            uint8_t val = ln_packet->data[x];
            // Print a leading 0 if less than 16 to make 2 HEX digits
            if (val < 16)
                Serial << F("0");

            Serial << HEX << unsigned(val) << " ";
          //  Serial.print(val, HEX);
          //  Serial.print(' ');
        }


        // TODO: should the endl be an F()?
        Serial << endl;

        loconet_.processSwitchSensorMessage(ln_packet);

    }
}

void Mrrwa_loconet_adapter::transmit_loop()
{

}


void Mrrwa_loconet_adapter::send_global_power_on_loop()
{
    if(send_gp_on_time_ms_) {

        if(get_time_ms() > send_gp_on_time_ms_) {

            if(true == loconet_.reportPower(1)) {
                send_gp_on_time_ms_ = 0;
            }
            else {
                send_gp_on_time_ms_ = get_time_ms() + POWER_ON_DELAY_MS;
            }
        }
    }

}

void Mrrwa_loconet_adapter::print_sensors() const
{
    for(Loconet_sensor* sensor : sensors_) {
        Serial << sensor->get_name() << F(" (#") << sensor->get_address() << F("): ") << sensor->is_active() << F(" (ind : ") << sensor->is_indeterminate() << F(")\n");
    }
}


bool queue_loconet_msg(lnMsg *msg)
{

}



}   // namespace mr_signals


#if 0


void MRRWALocoNetAdapter::QueueLocoNetMsg(lnMsg *msg)
{
    unsigned char msg_len = getLnMsgSize(msg);

    if(remaining_space(&ln_tx_queue) >= msg_len)
    {
        for(unsigned char i = 0; i<msg_len;i++)
        {
            enqueue(&ln_tx_queue,msg->data[i]);
        }
    }
}



RunTimeMs MRRWALocoNetAdapter::GetTimeMs()
{
#ifdef _MSC_VER
    // Fill in

#else
    return millis();

#endif //
}


#endif
