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


/**
 * Set function to provide a reference to the adapter for use by C
 * functions in the .cpp file that are required by the MRRWA package
 * @param adapter
 */
void set_mrrwa_loconet_adapter(mr_signals::Mrrwa_loconet_adapter * const adapter)
{
    loconet_adapter = adapter;
}




// Callback for the MRRWA LocoNet module (called from LocoNet.processSwitchSensorMessage)
// for all Sensor messages
void notifySensor(uint16_t Address, uint8_t State)
{

#ifdef ARDUINO
    Serial << F("Sensor: ") << Address << F(" - ") << (State ? F("Active") : F("Inactive"));
#else
    Serial << F("Sensor: ") << std::dec << Address << F(" - ") << (State ? F("Active") : F("Inactive"));
#endif

    if(nullptr!= loconet_adapter) {
        loconet_adapter->notify_sensors((mr_signals::Loconet_address)Address, State ? true : false);
    }
}

void notifySwitchRequest(uint16_t Address, uint8_t output, uint8_t direction) {
#ifdef ARDUINO

    if(Address >= 1017 && Address <=1020) {
        Serial << F("Sensor Interrogation");
    }
    else {
        Serial << F("Switch: ") << Address << F(" - ") << (direction ? F("Closed") : F("Thrown"));
    }

    Serial << F(" (Output ") << (output ? F("On") : F("Off")) << F(")");

#else

#endif
}
/*
  case OPC_SW_REQ:
    if(notifySwitchRequest)
      notifySwitchRequest( Address, LnPacket->srq.sw2 & OPC_SW_REQ_OUT, LnPacket->srq.sw2 & OPC_SW_REQ_DIR ) ;
    break ;
*/


namespace mr_signals {


Mrrwa_loconet_adapter::Mrrwa_loconet_adapter(LocoNetClass& loconet,int tx_pin, size_t num_sensors, size_t tx_buffer_size) :
        sensor_init_size_(num_sensors), next_tx_time_ms_(0), send_gp_on_time_ms_(0), next_tx_window_time_(0),msg_tx_window_count_(0),
        tx_errors_(0), long_acks_(0), loconet_(loconet), tx_pin_(tx_pin), any_sensor_indeterminate_(true)
{

    if(num_sensors) {
        sensors_.reserve(num_sensors);
    }

    tx_buffer_.initialize(tx_buffer_size);

    // Register the adapter with the pointer used by the MRRWA callbacks
    ::set_mrrwa_loconet_adapter(this);

    send_gp_on_time_ms_ = get_time_ms() + POWER_ON_DELAY_MS;

}

void Mrrwa_loconet_adapter::setup()
{
    loconet_.init(tx_pin_);
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


bool Mrrwa_loconet_adapter::any_sensor_indeterminate()  {


    if(any_sensor_indeterminate_){
        for (auto sensor : sensors_) {
            if (sensor->is_indeterminate()) {
                return true;
            }
        }
    }

    any_sensor_indeterminate_=false;
    return(any_sensor_indeterminate_);
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

bool Mrrwa_loconet_adapter::insert_ln_tx_delay(uint8_t delay_ms) {

    lnMsg SendPacket ;

    // Reuse existing Idle OpCode (which is never transmitted) to indicate the insertion of a delay
    SendPacket.data[ 0 ] = OPC_IDLE ;
    SendPacket.data[ 1 ] = delay_ms ;

    return(tx_buffer_.queue_loconet_msg(SendPacket));
}


Runtime_ms Mrrwa_loconet_adapter::get_time_ms() const
{
    return millis();
}

size_t Mrrwa_loconet_adapter::sensor_count()
{
    return sensors_.size();
}

size_t Mrrwa_loconet_adapter::sensor_init_size()
{
    return sensor_init_size_;
}




void Mrrwa_loconet_adapter::notify_sensors(Loconet_address address, bool state) const
{

    std::find_if(sensors_.begin(),
        sensors_.end(),
        [address, state](Loconet_sensor * sensor) {

        if (sensor->notify(address, state)) {
            Serial << F("\nSet Sensor ") << sensor->get_name() << " -> " << (state ? F("Active") : F("Inactive"));

            return true;    // Found the sensor, stop the find_if() loop
        }
        else {
            return false;   // Continue the find_if() loop
        }
    });
}

void Mrrwa_loconet_adapter::print_lnMsg(lnMsg *ln_packet, const char *prefix, bool print_checksum)
{
    char timestamp[12];
    sprintf(timestamp,"%08lu",get_time_ms());   // TODO: Why does this throw a warning in Eclipse of being a unsigned int??.  Look at http://arduiniana.org/libraries/streaming/

    Serial << timestamp << ":";

    Serial << prefix << " ";

    uint8_t msg_len = getLnMsgSize(ln_packet);

    if(!print_checksum) { msg_len--; }   // If not requested, don't print the checksum byte

    for (uint8_t x = 0; x < msg_len; x++)
    {
        uint8_t val = ln_packet->data[x];
        // Print a leading 0 if less than 16 to make 2 HEX digits
        if (val < 16)
            Serial << F("0");

#ifdef ARDUINO

        // TODO: Fix this mess.  HEX stream doesn't work in Arduino
        Serial.print(val,HEX);
        Serial.print(' ');
#else
        Serial << HEX << unsigned(val) << F(" "); // Works in Windows, garbage in arduino
      //  Serial << HEX << val << F(" ");
#endif
    }

    if(!print_checksum) {
        Serial << F("cs "); // If not printing the checksum, print 'cs' to align with when it is
    }

    if(OPC_LONG_ACK == ln_packet->data[0]) {
        Serial << F(" LONG_ACK!");
        next_tx_time_ms_ += 100;
    }


    // Let the calling function add any desired decoding before the CR/LF
//    Serial << endl;
}



void Mrrwa_loconet_adapter::receive_loop()
{
    lnMsg        *ln_packet;


    ln_packet = loconet_.receive();

    if(nullptr != ln_packet) {

        print_lnMsg(ln_packet,"LN RX",true);

        if(OPC_LONG_ACK == ln_packet->data[0]) {
            long_acks_++;
        }

        loconet_.processSwitchSensorMessage(ln_packet);

        Serial << endl;  // Clean up formatting
    }
}

void Mrrwa_loconet_adapter::transmit_loop()
{
    lnMsg ln_msg;

    if(get_time_ms() >= next_tx_time_ms_) {

        if(tx_buffer_.dequeue_loconet_msg(ln_msg)) {



            if(OPC_IDLE == ln_msg.data[0]) {
                // Treat as inserted delay with the delay duration being in the second byte
                next_tx_time_ms_ = get_time_ms() + ln_msg.data[1];
            }

            else {

                print_lnMsg(&ln_msg,"LN TX",false);

                if(LN_DONE != loconet_.send(&ln_msg)) {
                    tx_errors_++;
                    Serial << "-TX error" << endl;
                }
                else {
                    Serial << endl;
                }

                msg_tx_window_count_++;

                next_tx_time_ms_ = get_time_ms() + transmit_delay_ms_;
            }
        }
    }


    if(get_time_ms() >= next_tx_window_time_) {


        if(msg_tx_window_count_ >= 10) {
            // This means that messages are being sent very rapidly; insert a delay
            next_tx_time_ms_ += 500;
            Serial << endl << F("!!Inserting Tx delay due to high tx rate") << endl << endl;
        }


        msg_tx_window_count_ = 0;

        next_tx_window_time_ = get_time_ms() + tx_window_duration_ms_;
    }

}


void Mrrwa_loconet_adapter::send_global_power_on_loop()
{
    if(send_gp_on_time_ms_) {

        if(get_time_ms() > send_gp_on_time_ms_) {

            //if(true == loconet_.reportPower(1)) {
            if(true == send_opc_gp_on()) {  //## TODO - why wasn't the true condition error found in UT?
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

