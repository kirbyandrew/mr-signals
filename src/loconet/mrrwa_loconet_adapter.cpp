/*
 * mrrwa_loconet_adapter.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */

#include "mrrwa_loconet_adapter.h"

#ifndef ARDUINO
#include "arduino_mock.h"   // millis()
#endif


namespace mr_signals {


Mrrwa_loconet_adapter::Mrrwa_loconet_adapter(LocoNetClass& loconet) :
        send_gp_on_time_ms_(0), loconet_(loconet)
{


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




//    void queue_loconet_msg(lnMsg *msg);

    //void PrintSensors();

void Mrrwa_loconet_adapter::notify_sensors(Loconet_address address, bool state)
{

}



void Mrrwa_loconet_adapter::receive_loop()
{
    lnMsg        *LnPacket;

    LnPacket = loconet_.receive();
    if (nullptr != LnPacket) {
#if 0

        // First print out the packet in HEX
        Serial.print("RX: ");

        uint8_t msgLen = getLnMsgSize(LnPacket);
        for (uint8_t x = 0; x < msgLen; x++)
        {
            uint8_t val = LnPacket->data[x];
            // Print a leading 0 if less than 16 to make 2 HEX digits
            if (val < 16)
                Serial.print('0');

            Serial.print(val, HEX);
            Serial.print(' ');
        }

        // If this packet was not a Switch or Sensor Message then print a new line
        LocoNet.processSwitchSensorMessage(LnPacket);
        Serial.println();
#endif

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


}



