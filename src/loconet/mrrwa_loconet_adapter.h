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

#ifdef ARDUINO

    #include "LocoNet.h"    // The MRRWA package

#else

    #include "mrrwa_loconet_mock.h"  // Assume mock instance of LocoNet needed

#endif


//#include "LocoNet.h"

namespace mr_signals {

class Loconet_sensor;   // Forward definition for Mrrwa_loconet_adapter


class Mrrwa_loconet_adapter : public Loconet_adapter_interface
{
public:


    Mrrwa_loconet_adapter(LocoNetClass& loconet);

    bool setup(int tx_pin, int rx_pin) override;
    void loop() override;
    void attach_sensor(Loconet_sensor* sensor) override;
//    void DebugPrint();

    bool send_opc_sw_req(Loconet_address address, bool thrown,bool on) override;
    bool send_opc_gp_on() override;


    Runtime_ms get_time_ms();

//    void queue_loconet_msg(lnMsg *msg);

    //void PrintSensors();

    void notify_sensors(uint16_t address, bool state);


private:

    void receive_loop();
    void transmit_loop();
//    bool QueueLocoNetMsg(lnMsg *msg);

    std::vector<Loconet_sensor*> sensors_;

    long send_gp_on_time_ms_;  // Next time from get_time_ms() to send the Global Power On message

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


#endif /* SRC_LOCONET_MRRWA_LOCONET_ADAPTER_H_ */
