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


using namespace mr_signals;

using ::testing::AtLeast;
using ::testing::Return;


TEST(MrrwaAdapter,BasicMappingCalls)
{
    const uint8_t tx_pin=2;
    LocoNetMock loconet_mock;

    EXPECT_CALL(loconet_mock,init(tx_pin));
    EXPECT_CALL(loconet_mock,reportPower(1)).WillOnce(Return(LN_DONE));


    Mrrwa_loconet_adapter loconet_adapter(loconet_mock);

    EXPECT_TRUE(loconet_adapter.setup(tx_pin,0));
    EXPECT_TRUE(loconet_adapter.send_opc_gp_on());
}
