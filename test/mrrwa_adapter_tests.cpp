/*
 * mrrwa_adapter_tests.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */


#include <iostream>

#include "mrrwa_loconet_adapter.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "arduino_mock.h"


using namespace mr_signals;

using ::testing::AtLeast;
using ::testing::Return;




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

    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));

    // Setup so that the first call to reportPower fails, then the next succeeds
    EXPECT_CALL(loconet_mock,reportPower(1))
        .WillOnce(Return(LN_NETWORK_BUSY))
        .WillOnce(Return(LN_DONE));

    // Run for long enough to allo the reportPower(1) be called twice
    while(set_millis(millis()+1)  < (POWER_ON_DELAY_MS*3) ) {

        loconet_adapter.loop();
    }

}

