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
 * Bitmask for inserting transmit delays in the LN tx buffer which otherwise
 * uses raw LocoNet messages with the length and function using the LN OPC.
 *
 * It fits into an unused bit in the OPCs that are added to the LN tx buffer
 * for all except the Write Slot Data, which just needs to be checked before
 * checking for the Delay message
 *
 * 8x  1xxx xxxx       Power OPCs (not added to LN tx buffer)
 * Ax  1010 xxxx       Loco OPCs  (not used)
 * Bx  1011 xxxx       Most OPCs (used)
 * Ex  1110 xxxx       System OPCs (only EF may be used)
 * EF  1110 1111       Write Slot Data (OPC_WR_SL_DATA)
 */


//#define NON_LN_DELAY_MSG 0x40
//#define NON_LN_DELAY_MASK 0x3F

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


Mrrwa_loconet_adapter::Mrrwa_loconet_adapter(LocoNetClass& loconet,size_t num_sensors, size_t tx_buffer_size) :
        next_tx_time_ms_(0), send_gp_on_time_ms_(0), tx_errors_(0), loconet_(loconet)
{
    if(num_sensors) {
        sensors_.reserve(num_sensors);
    }

    tx_buffer_.initialize(tx_buffer_size);
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

    transmit_loop();

    send_global_power_on_loop();
}


void Mrrwa_loconet_adapter::attach_sensor(Loconet_sensor* sensor)
{
    sensors_.push_back(sensor);
}


bool Mrrwa_loconet_adapter::send_opc_sw_req(Loconet_address address, bool thrown,bool on)
{
    lnMsg SendPacket ;

    uint8_t sw2 = 0x00;
    if (!thrown) { sw2 |= OPC_SW_REQ_DIR; }
    if (on) { sw2 |= OPC_SW_REQ_OUT; }
    sw2 |= ((address-1) >> 7) & 0x0F;

    SendPacket.data[ 0 ] = OPC_SW_REQ ;
    SendPacket.data[ 1 ] = (address-1) & 0x7F ;
    SendPacket.data[ 2 ] = sw2 ;

    return(tx_buffer_.queue_loconet_msg(SendPacket));
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
    lnMsg ln_msg;

    if(get_time_ms() >= next_tx_time_ms_) {

        if(tx_buffer_.dequeue_loconet_msg(ln_msg)) {

            if(LN_DONE != loconet_.send(&ln_msg)) {
                tx_errors_++;
            }

            next_tx_time_ms_ = get_time_ms() + transmit_delay_ms_;
        }
    }
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



///////////////////////////////////////////////////


bool Mrrwa_loconet_tx_buffer::initialize(std::size_t buffer_size)
{
    return(loconet_tx_buffer_.initialize(buffer_size));
}


/**
 * Queues a LocoNet message for transmission onto LocoNet
 * *
 * First the length of the message is extracted from it using the MRWWA's getLnMsgSize function
 *
 * Then sanity checks on the size of the message are performed.  It is critical that the
 * loop_transmit() function can extract exactly as many bytes are queued in this function
 * to retain coherency as the LocoNet messages are encoded in the raw without any
 * container to allow a recovery from invalid data being encoded.  The loop_transmit function
 * can ignore invalid data, but it has to extract exactly the right number of bytes.
 *
 * The sanity checks include:
 *      Message not too big (larger than lnMsg)
 *      Message is at least 2 bytes long (minimum LN message)
 *
 * After the validity checks, it is checked that the message will fit into
 * loconet_tx_buffer_.
 *
 * If all checks pass, the LocoNet message is added to the loconet_tx_buffer_.
 *
 *
 * @param msg  LN Message to queue
 * @return  true if the message was queued, false otherwise
 */

bool Mrrwa_loconet_tx_buffer::queue_loconet_msg(lnMsg& msg)
{
    bool return_value = false;
    uint8_t msg_len = getLnMsgSize(&msg);

    // Messages longer than 2 bytes are encoded without the checksum
    // The checksum is retained for 2 byte messages as 2 bytes are
    // extracted for all messages as much longer messages encode
    // their length in the second byte
    if(msg_len > 2) {
        msg_len--;
    }

    if( msg_len <= sizeof(lnMsg) &&                     // Not too big
        msg_len >= 2 &&                                 // Not too small
        msg_len <= loconet_tx_buffer_.get_free()) {     // Can fit into the buffer


        for(uint8_t i=0;i<msg_len;i++) {
            (void) loconet_tx_buffer_.enqueue(msg.data[i]);    // Assume can enqueue if the get_free() above is large enough
        }

        return_value = true;
    }

    return (return_value);
}

/**
 * Dequeues a message to transmit
 *
 * If the transmit buffer is not empty, read the next Loconet message from it
 * The buffer is a stream of bytes; the length of each message is encoded in the
 * Loconet message itself.  This function is extremely dependent on valid
 * LocoNet messages being encoded into the buffer in queue_loconet_msg() to
 * ensure the correct length can be read out.
 *
 * @param msg - Loconet message to populate with the next in the queue
 * @return true if a message was dequeued, false if not
 */

bool Mrrwa_loconet_tx_buffer::dequeue_loconet_msg(lnMsg& msg)
{
    bool return_value = false;

    // Check if anything is in the buffer
    if(loconet_tx_buffer_.get_free() < loconet_tx_buffer_.max_size()) {

        // Pull the first two bytes; LN messages are always 2 or more bytes, and the command and
        // length are encoded in these first two messages
        loconet_tx_buffer_.dequeue(msg.data[0]);
        loconet_tx_buffer_.dequeue(msg.data[1]);

        // Read any remaining message (if longer than 2 bytes) from the queue
        // however for any messages > 2, do not read the checksum as it is
        // not encoded
        uint8_t msg_len = getLnMsgSize(&msg);

        for(uint8_t i=2; i<msg_len-1;i++) {
            loconet_tx_buffer_.dequeue(msg.data[i]);
        }
        return_value = true;
    }

    return (return_value);
}


}   // namespace mr_signals

