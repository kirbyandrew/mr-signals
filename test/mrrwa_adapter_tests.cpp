/*
 * mrrwa_adapter_tests.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */


#include <iostream>
#include <cstring>
#include <stdio.h>

#include "mrrwa_loconet_adapter.h"
#include "loconet_switch.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "arduino_mock.h"


using namespace mr_signals;

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;


extern void notifySensor(uint16_t , uint8_t );





// Copy of function from LocoNet.cpp from MRRWA library
uint8_t procSwitchSensorMessage( lnMsg *LnPacket )
{
    uint16_t Address ;
//    uint8_t  Direction ;
//    uint8_t  Output ;
    uint8_t  ConsumedFlag = 1 ;

    Address = (LnPacket->srq.sw1 | ( ( LnPacket->srq.sw2 & 0x0F ) << 7 )) ;
    if( LnPacket->sr.command != OPC_INPUT_REP )
      Address++;

    switch( LnPacket->sr.command )
    {
    case OPC_INPUT_REP:
      Address <<= 1 ;
      Address += ( LnPacket->ir.in2 & OPC_INPUT_REP_SW ) ? 2 : 1 ;

//      if(notifySensor)
        notifySensor( Address, LnPacket->ir.in2 & OPC_INPUT_REP_HI ) ;
      break;

    default:
      ConsumedFlag = 0 ;
    }

    return ConsumedFlag;
}


/*
 * Ensure that init(), reportPower() and setup() are called when the
 * loconet_adapter is first initialized.
 *
 * And that receive() is called as part of the regular loop
 */
TEST(MrrwaAdapter,BasicMappingCalls)
{
    const uint8_t tx_pin=2;
    LocoNetMock loconet_mock;

    Mrrwa_loconet_adapter loconet_adapter(loconet_mock);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);

    // Test the basic expected
    init_millis();

    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_CALL(loconet_mock,receive()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(loconet_mock,reportPower(1)).WillOnce(Return(LN_DONE));

    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));

    EXPECT_CALL(loconet_mock,receive()).WillRepeatedly(Return(nullptr));


    // Run for long enough to allo the reportPower(1) be called
    while(set_millis(millis()+1)  < (POWER_ON_DELAY_MS + (POWER_ON_DELAY_MS/2))) {

        loconet_adapter.loop();
    }
}

/*
 * Test that at least one retry for sending the power on message is attempted
 * after the loconet adapter is initialized.
 */
TEST(MrrwaAdapter,InitRetries)
{
    const uint8_t tx_pin=2;
    LocoNetMock loconet_mock;

    Mrrwa_loconet_adapter loconet_adapter(loconet_mock);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);


    init_millis();


    EXPECT_CALL(loconet_mock,init(tx_pin));

    // Setup so that the first call to reportPower fails, then the next succeeds
    EXPECT_CALL(loconet_mock,reportPower(1))
        .WillOnce(Return(LN_NETWORK_BUSY))
        .WillOnce(Return(LN_DONE));

    EXPECT_CALL(loconet_mock,receive()).WillRepeatedly(Return(nullptr));


    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));


    // Run for long enough to allo the reportPower(1) be called twice
    while(set_millis(millis()+1)  < (POWER_ON_DELAY_MS*3) ) {

        loconet_adapter.loop();
    }
}

/*
 * Test the MrrwaAdapater::receive_loop() function
 * If Loconet::receive returns NULL, Loconet::processSwitchSensorMessage()
 * should not be called
 */
TEST(MrrwaAdapter,NoMsgsReceivedLoopTest)
{
    const uint8_t tx_pin=2;
    LocoNetMock loconet_mock;

    Mrrwa_loconet_adapter loconet_adapter(loconet_mock);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);


    init_millis();

    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));

    EXPECT_CALL(loconet_mock,receive()).WillOnce(Return(nullptr));
    EXPECT_CALL(loconet_mock,processSwitchSensorMessage(_)).Times(0);

    loconet_adapter.loop();
}

/*
 * Test the MrrwaAdapater::receive_loop() function
 * If Loconet::receive returns non-null, processSwitchSensorMessage()
 * should be called, otherwise it should not be called
 */
TEST(MrrwaAdapter,MsgReceivedLoopTest)
{
    const uint8_t tx_pin=2;
    const int cycles = 5;       // Some number of loops to run
    LocoNetMock loconet_mock;

    Mrrwa_loconet_adapter loconet_adapter(loconet_mock);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);


    init_millis();

    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));

    lnMsg msg;

    // LocoNet::receive() will be called for each iteration of the cycle
    // Set it up so that it only returns non-null once
    EXPECT_CALL(loconet_mock,receive()).Times(cycles).WillOnce(Return(&msg)).WillRepeatedly(Return(nullptr));

    // LocoNet::processSwitchSensorMessage() should only be called once (for the one case that receive() returns non-nullptr
    EXPECT_CALL(loconet_mock,processSwitchSensorMessage(_)).Times(1);
    ON_CALL(loconet_mock,processSwitchSensorMessage(_)).WillByDefault(testing::Invoke(procSwitchSensorMessage));

    for(int i=0;i < cycles; i++)
    {
        loconet_adapter.loop();
    }
}


/*
 * Test the debug output by Mrrwa_loconet_adapter::receive_loop and
 * notifySensor() for specific received Loconet messages
 *
 * Full debug should be observed for the declared sensor, and not for
 * a sensor ID that doesn't have an associated sensor
 */

TEST(MrrwaAdapter,ReceivedDebugTest)
{
    const uint8_t tx_pin=2;
    LocoNetMock loconet_mock;

    Mrrwa_loconet_adapter loconet_adapter(loconet_mock);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);

    // Attach one sensor, ID 50
    Loconet_sensor sensor1("Sen1",50,loconet_adapter);

    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));

    lnMsg msg;

    // Construct Sensor report for sensor 50, state Active
    // An actual sensor exists for this (sensor1)
    msg.srp.command = 0xB2;
    msg.srp.sn1 = 0x18;
    msg.srp.sn2 = 0x70;
    msg.srp.chksum = 0x25 ;


    // LocoNet::receive() will be called for each iteration of the cycle
    EXPECT_CALL(loconet_mock,receive()).WillRepeatedly(Return(&msg));

    EXPECT_CALL(loconet_mock,processSwitchSensorMessage(_)).Times(2);
    ON_CALL(loconet_mock,processSwitchSensorMessage(_)).WillByDefault(testing::Invoke(procSwitchSensorMessage));


    std::cout << std::dec;

    // Capture stdout to compare debug output
    testing::internal::CaptureStdout();

    // Run loop to iterate loconet adapter
    loconet_adapter.loop();


    // Set To Sensor: 72 - Inactive
    // No sensor exits for this
    // RX: B2 23 70 1E Sensor: 72 - Active  (from log)
    msg.srp.command = 0xB2;
    msg.srp.sn1 = 0x23;
    msg.srp.sn2 = 0x60;
    msg.srp.chksum = 0x35 ;


    loconet_adapter.loop();


    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output = '" << output << "'" << std::endl;

    EXPECT_EQ("LN RX : B2 18 70 25 \nSensor Msg: 50 - Active\nSet Sensor Sen1 -> Active\n"\
              "LN RX : B2 23 60 35 \nSensor Msg: 72 - Inactive\n",output);

}

/* Test attaching sensors and that they receive signals
 *
 * Attach 3 sensors to a Mrrwa_loconet_adapter instance that is dimensioned for only 2
 * All 3 sensor should be supported (expected to allow sensors_ to grow if needed)
 *
 * Process the Loconet message to set the first and third sensors active; test
 * each to ensure that the processing loop touches the boundary sensors in the list (first and last)
 *
 * Finally process an 'inactive' LN message for a sensor that is active, confirm it goes inactive
 * */
TEST(MrrwaAdapter,AttachingSensors)
{
    const uint8_t tx_pin=2;
    LocoNetMock loconet_mock;
    int const sensor_count = 2;

    // Create an adapter object, dimension it to hold 2 sensors
    Mrrwa_loconet_adapter loconet_adapter(loconet_mock,sensor_count);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);


    // With no sensors attached, the count should be 0
    EXPECT_EQ((size_t)0,loconet_adapter.sensor_count());

    // Start adding sensors, checking that the count grows as expected
    Loconet_sensor sensor50("S50",50,loconet_adapter);
    EXPECT_EQ((size_t)1,loconet_adapter.sensor_count());

    Loconet_sensor sensor42("S42",42,loconet_adapter);
    EXPECT_EQ((size_t)2,loconet_adapter.sensor_count());

    // Should be able to add more sensors that we dimensioned loconet_adapter for (sensor_count)
    Loconet_sensor sensor72("S72",72,loconet_adapter);
    EXPECT_EQ((size_t)3,loconet_adapter.sensor_count());

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);


    lnMsg msg;


    // Load up the mocks
    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_CALL(loconet_mock,receive()).WillRepeatedly(Return(&msg));
    EXPECT_CALL(loconet_mock,processSwitchSensorMessage(_)).Times(3);
    ON_CALL(loconet_mock,processSwitchSensorMessage(_)).WillByDefault(testing::Invoke(procSwitchSensorMessage));

    // Init
    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));

    // All sensors should be inactive at this point
    EXPECT_FALSE(sensor50.is_active());
    EXPECT_FALSE(sensor42.is_active());
    EXPECT_FALSE(sensor72.is_active());



    // Construct Sensor report for sensor 50, state Active
    msg.srp.command = 0xB2;
    msg.srp.sn1 = 0x18;
    msg.srp.sn2 = 0x70;
    msg.srp.chksum = 0x25 ;



    // Process the above message; expect only the specific sensor (50) to go active
    loconet_adapter.loop();

    EXPECT_TRUE(sensor50.is_active());
    EXPECT_FALSE(sensor42.is_active());
    EXPECT_FALSE(sensor72.is_active());



    // RX: B2 23 70 1E Sensor: 72 - Active  (from log)
    msg.srp.command = 0xB2;
    msg.srp.sn1 = 0x23;
    msg.srp.sn2 = 0x70;
    msg.srp.chksum = 0x1E ;

    // Process new message. Expect the later sensor (72) to also go active
    loconet_adapter.loop();

    EXPECT_TRUE(sensor50.is_active());
    EXPECT_FALSE(sensor42.is_active());
    EXPECT_TRUE(sensor72.is_active());


    // Set To Sensor: 72 - Inactive
    msg.srp.command = 0xB2;
    msg.srp.sn1 = 0x23;
    msg.srp.sn2 = 0x60;
    msg.srp.chksum = 0x35 ;

    loconet_adapter.loop();

    EXPECT_TRUE(sensor50.is_active());
    EXPECT_FALSE(sensor42.is_active());
    EXPECT_FALSE(sensor72.is_active());
}

/*
 * Test the sensor debug output of the MRRRWA adapter
 * Print the following states
 * Active
 * Inactive
 * Indeterminate
 */
TEST(MrrwaAdapter,SensorDebug)
{
    LocoNetMock loconet_mock;
    int const sensor_count = 3;

    // Create an adapter object, dimension it to hold 2 sensors
    Mrrwa_loconet_adapter loconet_adapter(loconet_mock,sensor_count);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);


    Loconet_sensor sensor1("Sensor1",1,loconet_adapter);    // Ensure the constrained name string prints correctly
    Loconet_sensor sensor2("S2",2,loconet_adapter);
    Loconet_sensor sensor3("S0003",3,loconet_adapter);

    sensor1.set_state(true);            // Active sensor
    sensor2.set_state(false);           // Inactive sensor
    // Leave sensor 3 as indeterminate

    testing::internal::CaptureStdout();

    loconet_adapter.print_sensors();

    std::string output = testing::internal::GetCapturedStdout();

    std::cout << output;

    EXPECT_EQ(  "Senso (#1): 1 (ind : 0)\n"\
                "S2 (#2): 0 (ind : 0)\n"\
                "S0003 (#3): 0 (ind : 1)\n",output);

}

/*
 * Matcher to test the LocoNet send() mock function
 * Compares the first 3 bytes passed in the test with those passed to the
 * send() mock function
 */
MATCHER_P(test_3_byte_send, bytes, "") {

    uint8_t * dataToCheck = arg->data;

    bool isMatch = (memcmp(dataToCheck, bytes, 3) == 0);

/*
    std::cout << "test_3_byte_send" << std::endl;

    for(int i=0;i<3;i++) {
//        std::cout <<  std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(bytes[i]) << ":" << static_cast<int>(dataToCheck[i]) << std::endl;
        printf("%02X:%02X\n",bytes[i],dataToCheck[i]);
    }
*/
    return isMatch;
}

/* Test the basics of using the loconet adapater send functions, and that
 * the transmit loop pulls out enqueued messages
 *
 */
TEST(MrrwaAdapter,TxLoopTest)
{
    const uint8_t tx_pin=2;
    const std::size_t buffer_size = 8;
    LocoNetMock loconet_mock;

    Mrrwa_loconet_adapter loconet_adapter(loconet_mock,0,buffer_size);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);

    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));

    // Try to queue 3 messages.  With a buffer of 8, only two 3-byte long
    // messsages (OPC_SW_REQ is 4, the CRC is not stored) can be
    // enqueued
    EXPECT_EQ(0u,loconet_adapter.get_buffer_high_watermark());
    EXPECT_TRUE(loconet_adapter.send_opc_sw_req(0x123,true,true));
    EXPECT_EQ(3u,loconet_adapter.get_buffer_high_watermark());
    EXPECT_TRUE(loconet_adapter.send_opc_sw_req(0x123,true,false));
    EXPECT_EQ(6u,loconet_adapter.get_buffer_high_watermark());
    EXPECT_FALSE(loconet_adapter.send_opc_sw_req(0x123,false,true));
    EXPECT_EQ(6u,loconet_adapter.get_buffer_high_watermark());


    // Expected bytes for send_opc_sw_req(0x123,true,true)
    uint8_t test_bytes[3] = { 0xB0, 0x22, 0x12 };

    EXPECT_CALL(loconet_mock,receive()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(loconet_mock,send(test_3_byte_send(test_bytes))).Times(1).WillOnce(Return(LN_DONE)); // Should only be called once in the following

    set_millis(1);          // Set past 0 so that transmit_loop() will call send
    loconet_adapter.loop(); // will call send()

    loconet_adapter.loop(); // should not call send() as insufficient time has elapsed

    // should be no send errors
    EXPECT_EQ(0,loconet_adapter.get_tx_error_count());

    set_millis(11);
    test_bytes[2] = 0x02;   // Change the 2nd byte to reflect // Expected bytes for send_opc_sw_req(0x123,true,false)
    EXPECT_CALL(loconet_mock,send(test_3_byte_send(test_bytes))).Times(1).WillOnce(Return(LN_RETRY_ERROR)); // Set another 1 time
    loconet_adapter.loop(); // will call send()

    set_millis(20);
    loconet_adapter.loop(); // should not call send() as insufficient time has elapsed

    // High watermark should remain the same after all the dequeuing
    EXPECT_EQ(6u,loconet_adapter.get_buffer_high_watermark());

    // With one transmit error
    EXPECT_EQ(1u,loconet_adapter.get_tx_error_count());
}

/*
 * Test the Loconet_switch implementation
 *
 * When a switch direction is requested, a LocoNet message should be
 * sent with the 'on' flag; then 80ms later the same should be sent
 * with 'off'.
 *
 * If the switch direction is requested before the 'off' is sent (even
 * if it is the same direction) this is then sent with 'on'
 * and only one 'off' is sent (for the most recent switch direction)
 */

TEST(MrrwaAdapter,LocoNetSwitchTest)
{
    const uint8_t tx_pin=2;
    const std::size_t buffer_size = 8;
    LocoNetMock loconet_mock;

    Mrrwa_loconet_adapter loconet_adapter(loconet_mock,0,buffer_size);

    // Set the adapter so that the C callbacks from the MRRWA library can find it
    set_mrrwa_loconet_adapter(&loconet_adapter);

    Loconet_switch switch1(0x123,&loconet_adapter);


    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));
    EXPECT_CALL(loconet_mock,receive()).WillRepeatedly(Return(nullptr));


    uint8_t test_bytes[3] = { 0xB0, 0x22, 0x12 };   // OPC_SW_REQ Addr:0x123, thrown, on

    EXPECT_CALL(loconet_mock,send(test_3_byte_send(test_bytes))).Times(1).WillOnce(Return(LN_DONE)); // Should only be called once in the following

    switch1.request_direction(Switch_direction::thrown);


    set_millis(1);
    switch1.loop();
    loconet_adapter.loop();

    test_bytes[2] = 0x02;   // Change the 2nd byte to reflect 'off'
    EXPECT_CALL(loconet_mock,send(test_3_byte_send(test_bytes))).Times(1).WillOnce(Return(LN_DONE)); // Should only be called once in the following

    switch1.loop();
    loconet_adapter.loop();

    set_millis(79);         // Advance time, but not enough

    switch1.loop();
    loconet_adapter.loop();

    set_millis(80);

    switch1.loop(); // Now should call send() again
    loconet_adapter.loop();

    switch1.loop(); // Will not call it again
    loconet_adapter.loop();


    // Now call a ::closed after a ::throw, but before the 80ms passes
    // A ::closed should get called with the 'off' argument
    set_millis(200);

    test_bytes[2] = 0x12;   // Change the 2nd byte to represent ::thrown, 'on'
    EXPECT_CALL(loconet_mock,send(test_3_byte_send(test_bytes))).Times(1).WillOnce(Return(LN_DONE)); // Should only be called once in the following

    switch1.request_direction(Switch_direction::thrown);
    switch1.loop();
    loconet_adapter.loop();


    set_millis(240);

    test_bytes[2] = 0x32;   // Change the 2nd byte to represent ::closed, 'on'
    EXPECT_CALL(loconet_mock,send(test_3_byte_send(test_bytes))).Times(1).WillOnce(Return(LN_DONE)); // Should only be called once in the following
    switch1.request_direction(Switch_direction::closed);
    switch1.loop();
    loconet_adapter.loop();


    test_bytes[2] = 0x22;   // Change the 2nd byte to represent ::closed, 'off'
    EXPECT_CALL(loconet_mock,send(test_3_byte_send(test_bytes))).Times(1).WillOnce(Return(LN_DONE)); // Should only be called once in the following

    set_millis(320);

    switch1.loop(); // Now should call send() again
    loconet_adapter.loop();

}

/////////////////////////// Mrrwa_loconet_tx_buffer tests ////////////////////


/*
 * Test the enqueuing function
 */
TEST(MrrwaTxBuffer,Enqueue)
{
    Mrrwa_loconet_tx_buffer tx_buffer;
    lnMsg msg;

    const std::size_t buffer_size=5;

    tx_buffer.initialize(buffer_size);


    // Check the message limits (0,1 & > 16 are all invalid)
    msg.sz.command = 0x60;
    msg.sz.mesg_size = 0;

    EXPECT_FALSE(tx_buffer.queue_loconet_msg(msg));

    msg.sz.mesg_size = 1;
    EXPECT_FALSE(tx_buffer.queue_loconet_msg(msg));

    msg.sz.mesg_size = sizeof(msg)+1;
    EXPECT_FALSE(tx_buffer.queue_loconet_msg(msg));


    EXPECT_EQ(buffer_size,tx_buffer.loconet_tx_buffer_.get_free());


    // Enqueue a valid 4 byte message (3 bytes are encoded); there should be space
    msg.data[0] = OPC_SW_REQ;
    msg.data[1] = 12;
    msg.data[2] = 34;
  //msg.data[3] = Checksum which is created in MRRWA library

    EXPECT_TRUE(tx_buffer.queue_loconet_msg(msg));

    EXPECT_EQ(buffer_size-3,tx_buffer.loconet_tx_buffer_.get_free());


    // Attempt to enqueue the same message; as the buffer is only 5 bytes this should fail
    EXPECT_FALSE(tx_buffer.queue_loconet_msg(msg));
}


/*
 * Enqueue and dequeue a number of Loconet messages to confirm this functionality
 */

TEST(MrrwaTxBuffer,Dequeue)
{
    Mrrwa_loconet_tx_buffer tx_buffer;
    lnMsg msg1,msg2,msg3,read_msg;

    const std::size_t buffer_size=8;

    tx_buffer.initialize(buffer_size);


    EXPECT_FALSE(tx_buffer.dequeue_loconet_msg(read_msg));
    EXPECT_EQ(buffer_size,tx_buffer.loconet_tx_buffer_.get_free());


    // Enqueue two valid 4 byte message (3 bytes are stored for each)
    msg1.data[0] = OPC_SW_REQ;
    msg1.data[1] = 12;
    msg1.data[2] = 34;
    EXPECT_TRUE(tx_buffer.queue_loconet_msg(msg1));
    EXPECT_EQ(buffer_size-3,tx_buffer.loconet_tx_buffer_.get_free());


    msg2.data[0] = OPC_SW_REQ;
    msg2.data[1] = 56;
    msg2.data[2] = 78;
    EXPECT_TRUE(tx_buffer.queue_loconet_msg(msg2));
    EXPECT_EQ(buffer_size-6,tx_buffer.loconet_tx_buffer_.get_free());


    // Should not be able to enque msg2 again; not enough space
    EXPECT_FALSE(tx_buffer.queue_loconet_msg(msg2));
    EXPECT_EQ(buffer_size-6,tx_buffer.loconet_tx_buffer_.get_free());


    // Enqueue a 2 byte message
    msg3.data[0] = OPC_GPON;
    msg3.data[1] = 90;
    EXPECT_TRUE(tx_buffer.queue_loconet_msg(msg3));
    EXPECT_EQ(buffer_size-8,tx_buffer.loconet_tx_buffer_.get_free());





    // Pull the first message, confirm that it matches msg1 and that the free
    // space has increased
    std::memset(&read_msg,0x00,sizeof(lnMsg));

    EXPECT_TRUE(tx_buffer.dequeue_loconet_msg(read_msg));
    EXPECT_EQ(buffer_size-5,tx_buffer.loconet_tx_buffer_.get_free());

    EXPECT_EQ(0,std::memcmp(&read_msg,&msg1,3));


    // Pull the second message, confirm that it matches msg2 and that the free
    // space has increased
    std::memset(&read_msg,0x00,sizeof(lnMsg));

    EXPECT_TRUE(tx_buffer.dequeue_loconet_msg(read_msg));
    EXPECT_EQ(buffer_size-2,tx_buffer.loconet_tx_buffer_.get_free());

    EXPECT_EQ(0,std::memcmp(&read_msg,&msg2,3));


    // Pull the third message, confirm that it matches msg3 and that the free
    // space has increased
    std::memset(&read_msg,0x00,sizeof(lnMsg));

    EXPECT_TRUE(tx_buffer.dequeue_loconet_msg(read_msg));
    EXPECT_EQ(buffer_size,tx_buffer.loconet_tx_buffer_.get_free());

    EXPECT_EQ(0,std::memcmp(&read_msg,&msg3,2));
}
