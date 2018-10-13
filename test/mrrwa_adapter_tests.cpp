/*
 * mrrwa_adapter_tests.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */


#include <iostream>
#include <cstring>

#include "mrrwa_loconet_adapter.h"

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
 */

TEST(MrrwaAdapter,ReceivedDebugTest)
{
    const uint8_t tx_pin=2;
    LocoNetMock loconet_mock;

    Mrrwa_loconet_adapter loconet_adapter(loconet_mock);

    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));

    lnMsg msg;

    // Construct Sensor report for sensor 50, state Active
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
    // RX: B2 23 70 1E Sensor: 72 - Active  (from log)
    msg.srp.command = 0xB2;
    msg.srp.sn1 = 0x23;
    msg.srp.sn2 = 0x60;
    msg.srp.chksum = 0x35 ;


    loconet_adapter.loop();


    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output = '" << output << "'" << std::endl;

    EXPECT_EQ("LN RX : B2 18 70 25 \nSensor Msg: 50 - Active\n"\
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
