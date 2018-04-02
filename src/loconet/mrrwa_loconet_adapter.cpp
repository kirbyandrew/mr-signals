/*
 * mrrwa_loconet_adapter.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */

#include "mrrwa_loconet_adapter.h"


namespace mr_signals {


Mrrwa_loconet_adapter::Mrrwa_loconet_adapter(LocoNetClass& loconet) :
        send_gp_on_time_ms_(0), loconet_(loconet)
{


}

bool Mrrwa_loconet_adapter::setup(int tx_pin, int rx_pin)
{
    if (rx_pin) {}          // Not used in MRRWA LocoNet
    loconet_.init(tx_pin);


  //  send_gp_on_time_ms_ = GetTimeMs() + 200;


    return true;    // No return from MRRWA Loconet .init function, always return true here
}
void Mrrwa_loconet_adapter::loop()
{
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
return 0;
}

//    void queue_loconet_msg(lnMsg *msg);

    //void PrintSensors();

void Mrrwa_loconet_adapter::notify_sensors(Loconet_address address, bool state)
{

}



void Mrrwa_loconet_adapter::receive_loop()
{

}

void Mrrwa_loconet_adapter::transmit_loop()
{

}



}



