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
#include "single_switch_head.h"


using namespace mr_signals;


class Single_switch_sensor_test : public ::testing::Test {

protected:
    void SetUp()
    {
        head_ = new Single_switch_sensor_head("",switch_1_,sensor_1_);
    }

    void TearDown()
    {
        delete head_;
    }

    Single_switch_sensor_head* head_;
    Test_switch switch_1_;
    Sensor_base sensor_1_;
};

class Single_switch_test : public ::testing::Test {

protected:
    void SetUp(const std::string name)
    {
        head_ = new Single_switch_head(name.c_str(),test_switch_1_);
    }

    void TearDown()
    {
        delete head_;
    }

    Test_switch test_switch_1_;
    Single_switch_head* head_;

};


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

TEST_F(Single_switch_sensor_test,SwitchStates)
{
    SetUp();

    // Run these tests twice; once with the sensor indeterminate and
    // once with it set to false
    for(int i=0;i<2;i++) {
        switch_1_.request_direction(switch_thrown);

        // yellow & green should not be accepted while sensor is indeterminate or inactive
        EXPECT_FALSE(head_->set_aspect(yellow));
        EXPECT_EQ(switch_thrown,switch_1_.get_direction());
        EXPECT_FALSE(head_->set_aspect(green));
        EXPECT_EQ(switch_thrown,switch_1_.get_direction());

        // red and dark should set in all cases
        EXPECT_TRUE(head_->set_aspect(red));
        EXPECT_EQ(switch_closed,switch_1_.get_direction());

        switch_1_.request_direction(switch_thrown);
        EXPECT_TRUE(head_->set_aspect(dark));
        EXPECT_EQ(switch_closed,switch_1_.get_direction());

        sensor_1_.set_state(false);
    }

    // With the sensor active, all states should be accepted and change
    // the state of the switch
    sensor_1_.set_state(true);

    switch_1_.request_direction(switch_thrown);
    EXPECT_TRUE(head_->set_aspect(red));
    EXPECT_EQ(switch_closed,switch_1_.get_direction());

    EXPECT_TRUE(head_->set_aspect(yellow));
    EXPECT_EQ(switch_thrown,switch_1_.get_direction());

    EXPECT_TRUE(head_->set_aspect(dark));
    EXPECT_EQ(switch_closed,switch_1_.get_direction());

    EXPECT_TRUE(head_->set_aspect(green));
    EXPECT_EQ(switch_thrown,switch_1_.get_direction());
}

TEST_F(Single_switch_test,SwitchStates)
{
    SetUp("");
    EXPECT_EQ(switch_unknown,test_switch_1_.get_direction());
    EXPECT_EQ(unknown,head_->get_aspect());


    EXPECT_TRUE(head_->set_aspect(red));
    EXPECT_EQ(switch_closed,test_switch_1_.get_direction());

    EXPECT_TRUE(head_->set_aspect(red));
    EXPECT_EQ(switch_closed,test_switch_1_.get_direction());

    EXPECT_TRUE(head_->set_aspect(yellow));
    EXPECT_EQ(switch_thrown,test_switch_1_.get_direction());

    EXPECT_TRUE(head_->set_aspect(dark));
    EXPECT_EQ(switch_closed,test_switch_1_.get_direction());

    EXPECT_TRUE(head_->set_aspect(green));
    EXPECT_EQ(switch_thrown,test_switch_1_.get_direction());
}

TEST_F(Single_switch_test,HeldStates)
{
    SetUp("");
    // Check hold request is rejected if head's aspect is unknown (default)
    head_->set_held(true);
    EXPECT_FALSE(head_->is_held());

    // Check hold request is accepted if aspect is red
    EXPECT_TRUE(head_->set_aspect(red));
    EXPECT_FALSE(head_->is_held());
    head_->set_held(true);
    EXPECT_TRUE(head_->is_held());

    // Head remains held if the same aspect is requestd
    EXPECT_TRUE(head_->set_aspect(red));
    EXPECT_TRUE(head_->is_held());

    // Head should reject all changes while held, and still report red
    EXPECT_FALSE(head_->set_aspect(yellow));
    EXPECT_FALSE(head_->set_aspect(green));
    EXPECT_FALSE(head_->set_aspect(dark));


    EXPECT_TRUE(head_->is_held());
    EXPECT_EQ(red,head_->get_aspect());

    // Setting the held aspect should report success
    EXPECT_TRUE(head_->set_aspect(red));


    // Clearing the hold should not change the aspect state, and the mast
    // should now accept changes
    head_->set_held(false);
    EXPECT_FALSE(head_->is_held());
    EXPECT_EQ(red,head_->get_aspect());

    EXPECT_TRUE(head_->set_aspect(green));
    EXPECT_EQ(green,head_->get_aspect());
    EXPECT_FALSE(head_->is_held());

}


TEST_F(Single_switch_test,LongNameCheck)
{
    SetUp("Head1_");
    EXPECT_STREQ("Head1",head_->get_name());        // Expect truncated
}

TEST_F(Single_switch_test,ShortNameCheck)
{
    SetUp("a");
    EXPECT_STREQ("a",head_->get_name());        // Expect truncated
}

TEST_F(Single_switch_test,EmptyNameCheck)
{
    SetUp("");
    const char* name = head_->get_name();
    EXPECT_LT(strlen(name),5);        // Expect truncated
}



TEST(AspectNameStringTest, gets) {
    EXPECT_STREQ("u",get_aspect_string(unknown));

    EXPECT_STREQ("d",get_aspect_string(dark));
    EXPECT_STREQ("r",get_aspect_string(red));
    EXPECT_STREQ("y",get_aspect_string(yellow));
    EXPECT_STREQ("g",get_aspect_string(green));
    EXPECT_STREQ("e",get_aspect_string((Head_aspect)20));  // something invalid
}



TEST_F (Test_switch_test, TestSwitchBehavior){

    EXPECT_EQ(switch_unknown,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    EXPECT_TRUE(test_switch_1_.request_direction(switch_thrown));
    EXPECT_EQ(switch_thrown,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    EXPECT_TRUE(test_switch_1_.request_direction(switch_closed));
    EXPECT_EQ(switch_closed,test_switch_1_.get_direction());
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


