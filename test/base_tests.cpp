/*
 * base_tests.cpp
 *
 * Implements unit tests for all of the mr-signals base & interface classes
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */


#include "gtest/gtest.h"

#include "sensor_interface.h"
#include "head_interface.h"
#include "switch_interface.h"



using namespace mr_signals;



class Test_switch_test : public ::testing::Test {

    // virtual void SetUp() {}
    // virtual void TearDown() {}

protected:
    Test_switch test_switch_1_;
//    Test_switch test_switch_2_(2);

};

// Test fixture for the Active_sensor class
class Active_sensor_test: public ::testing::Test {
protected:

    // virtual void SetUp() {}
    // virtual void TearDown() {}

    Active_sensor active_sensor_;
};

//
class Base_sensor_test: public ::testing::Test {
protected:

    // virtual void SetUp() {}
    // virtual void TearDown() {}

    Sensor_base base_sensor_1_;
    Sensor_base base_sensor_2_;
};


TEST_F (Test_switch_test, TestSwitchBehavior){

    EXPECT_EQ(SWITCH_UNKNOWN,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    EXPECT_TRUE(test_switch_1_.request_direction(SWITCH_THROWN));
    EXPECT_EQ(SWITCH_THROWN,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    EXPECT_FALSE(test_switch_1_.request_direction(SWITCH_THROWN));
    EXPECT_EQ(SWITCH_THROWN,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    EXPECT_TRUE(test_switch_1_.request_direction(SWITCH_CLOSED));
    EXPECT_EQ(SWITCH_CLOSED,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    test_switch_1_.loop();
    EXPECT_EQ(1,test_switch_1_.get_loop_cnt());
}


/**
 * @brief
 * @param &sensor reference to the base sensor to test
 * @param state The state to set the sensor to
 * @param change_expected Indicates whether the sensor should report that setting the passed state indicates a change in its state
 *
 */
void sensor_base_set_state_test(Sensor_base& sensor, bool state,
        bool change_expected) {

    //EXPECT_EQ(change_expected, sensor.set_state(state));

    // EXPECT_EQ does not appear to evaluate true particularly consistently; looks like
    // it is cast to int internally and different non-0 values *sometimes* get returned
    // and the test will fail
    if(change_expected){
        EXPECT_TRUE(sensor.set_state(state));
    }
    else {
        EXPECT_FALSE(sensor.set_state(state));
    }


    EXPECT_EQ(false, sensor.is_indeterminate());
    EXPECT_EQ(state , sensor.get_state());
}

// Test the initialization and then changing of states of the base sensor
// Use two objects to test different order (true-false-true vs false-true-false)
TEST_F(Base_sensor_test, TestStates) {

    // First check that both sensors start as indeterminate
    EXPECT_TRUE(base_sensor_1_.is_indeterminate());
    EXPECT_TRUE(base_sensor_2_.is_indeterminate());


    // Set sensor 1 true->true->false->true
    sensor_base_set_state_test(base_sensor_1_,true,true);   // Expect change when first set to true
    sensor_base_set_state_test(base_sensor_1_,true,false);  // No change expected if set to true again
    sensor_base_set_state_test(base_sensor_1_,false,true);  // Expect change when set to false
    sensor_base_set_state_test(base_sensor_1_,true,true);   // Expect change when set to true

    // Set sensor 2 false->false->true->false
    sensor_base_set_state_test(base_sensor_2_,false,true);      // Expect change when first set to false
    sensor_base_set_state_test(base_sensor_2_,false,false);     // No change expected if set to false again
    sensor_base_set_state_test(base_sensor_2_,true,true);       // Expect change when set to true
    sensor_base_set_state_test(base_sensor_2_,false,true);      // Expect change when set to false

}


TEST_F(Active_sensor_test, IsEmptyInitially) {
  EXPECT_EQ(false, active_sensor_.is_indeterminate());
  EXPECT_EQ(true, active_sensor_.get_state());
}


