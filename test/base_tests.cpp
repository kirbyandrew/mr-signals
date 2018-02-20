/*
 * base_tests.cpp
 *
 * Implements unit tests for all of the mr-signals base & interface classes
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */


#include <ryg_logic.h>
#include "gtest/gtest.h"

#include "sensor_interface.h"
#include "head_interface.h"
#include "switch_interface.h"
#include "single_switch_head.h"
#include "double_switch_head.h"
#include "quadln_s_head.h"
#include "sensor_interlocked_head.h"

using namespace mr_signals;



class Quadln_s_test : public ::testing::Test {

protected:
    void SetUp()
    {
        head_ = new Quadln_s_head("",test_switch_1_,midpoint_switch_);
    }

    void TearDown()
    {
        delete head_;
    }

    Quadln_s_head* head_;
    Test_switch test_switch_1_;
    Test_switch midpoint_switch_;
};


class Double_switch_test : public ::testing::Test {

protected:
    void SetUp()
    {
        head_ = new Double_switch_head("",test_switch_1_,test_switch_2_);
    }

    void TearDown()
    {
        delete head_;
    }

    Double_switch_head* head_;
    Test_switch test_switch_1_;
    Test_switch test_switch_2_;
};


/*
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
*/
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


TEST(LeverWithPushKey,BasicStates)
{
//    Test_sensor lever_(false);
//    Test_sensor push_key_(false);
    Test_sensor lever_;
    Test_sensor push_key_;
    Lever_with_pushkey interlocked_lever_(lever_,push_key_);

    EXPECT_FALSE(interlocked_lever_.get_state());

    EXPECT_TRUE(interlocked_lever_.is_indeterminate());
    lever_.set_state(false);
    EXPECT_TRUE(interlocked_lever_.is_indeterminate());
    push_key_.set_state(false);
    EXPECT_FALSE(interlocked_lever_.is_indeterminate());


    lever_.set_state(true);
    EXPECT_FALSE(interlocked_lever_.get_state());
    EXPECT_FALSE(interlocked_lever_.get_state());

    push_key_.set_state(true);
    EXPECT_FALSE(interlocked_lever_.get_state());
    EXPECT_FALSE(interlocked_lever_.get_state());

    lever_.set_state(false);
    EXPECT_FALSE(interlocked_lever_.get_state());
    EXPECT_FALSE(interlocked_lever_.get_state());

    lever_.set_state(true);
    EXPECT_TRUE(interlocked_lever_.get_state());
    EXPECT_TRUE(interlocked_lever_.get_state());

}

TEST(Interlocked_ryg_test,CallOnButton)
{
    Test_sensor lever_(true);
    Test_sensor push_key_(false);
    Test_sensor sensor_1_(false);
    Test_switch test_switch_1_;

// Option1: Needs the head set to ::red and held to work
//  Single_switch_sensor_head head_("",test_switch_1_,push_key_);
//    Interlocked_ryg_logic test_logic_(head_, lever_, { &sensor_1_ });
    //  head_.request_aspect(Head_aspect::red);
    //  head_.set_held(true);


// Option2: Use Sensor_interlocked_head
//    Test_head head_;
//    Sensor_interlocked_head logical_head_(head_,push_key_);
//    Interlocked_ryg_logic test_logic_(logical_head_,lever_,{ &sensor_1_ });

// Option3 : Use Lever_with_pushkey
    Test_head head_;
    Lever_with_pushkey interlocked_lever_(lever_,push_key_);
    Interlocked_ryg_logic test_logic_(head_,interlocked_lever_,{ &sensor_1_ });



    // If the lever is reversed but the pushkey isn't pressed,
    // the head should remain red
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    // Pushing the key with the lever already reversed will not
    // clear the head (already locked to red)
    push_key_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());

    // Set the lever to normal
    lever_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());


    // And then reverse it again with the push key pressed
    // and the head should go green
    lever_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());

    // The head will stay green if the pushkey is then released
    push_key_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());


    lever_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());

    lever_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());

    push_key_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());

}


TEST(Interlocked_ryg_test,AutomatedLever)
{
    Test_head head_;
    Test_head protected_head_;
    Test_sensor sensor_1_(false);
    Test_sensor automated_lever_(false);
    Test_sensor lever_(false);

    Interlocked_ryg_logic test_logic_(head_, protected_head_, lever_,
            automated_lever_, { &sensor_1_ });

    // If the lever isn't reversed, the head should go red
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    // Reverse the lever, should go green
    lever_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());

    // If the sensor goes active, the head will go and stay red
    // even if the sensor goes inactive
    sensor_1_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    sensor_1_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    // Even when the automated lever is reversed, the head will stay
    // locked on red
    automated_lever_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    // Set lever to normal and then back to reversed, and the head
    // should return to green
    lever_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    lever_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());

    // Now if the sensor goes active, the head will go red,
    // but once the sensor goes inactive, it will go back to
    // green due to the automated lever being reversed
    sensor_1_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    sensor_1_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());

    // Check that the protected head logic works with the
    // auotmated lever reversed
    protected_head_.request_aspect(Head_aspect::red);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::yellow,head_.get_aspect());

    protected_head_.request_aspect(Head_aspect::yellow);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());

    protected_head_.request_aspect(Head_aspect::green);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());


}



TEST(Interlocked_ryg_test,BasicInterlocking)
{
    Test_head head_;
    Test_head protected_head_;
    Sensor_base sensor_1_;
    Sensor_base lever_;

    Interlocked_ryg_logic test_logic_(head_,protected_head_,lever_,{&sensor_1_});


    protected_head_.request_aspect(Head_aspect::green);
    sensor_1_.set_state(false);

    test_logic_.loop();
    // Lever is indeterminate, head state will not change
    EXPECT_EQ(Head_aspect::unknown,head_.get_aspect());

    // once the lever state is known, the head will go red
    lever_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    // lever reversed, no blocking elements, should go green
    lever_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());

    // Check that the expected protected head logic works
    protected_head_.request_aspect(Head_aspect::red);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::yellow,head_.get_aspect());

    protected_head_.request_aspect(Head_aspect::yellow);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());

    protected_head_.request_aspect(Head_aspect::green);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());


    // Protected sensor active, drops head to red
    sensor_1_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    // Once set to red, the head should stay there even if the sensor goes inactive
    sensor_1_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    // Should require the lever to be set to normal and then reversed
    // again (with clear conditions) for the head to change from red
    lever_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red,head_.get_aspect());

    lever_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::green,head_.get_aspect());
}


TEST(Simple_ryg_test,ProtectHeadAndSensor)
{
    Test_head head_;
    Test_head protected_head_;
    Sensor_base sensor_1_;

    Simple_ryg_logic test_mast_(head_,protected_head_,{&sensor_1_});


    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    EXPECT_EQ(Head_aspect::unknown, protected_head_.get_aspect());
    EXPECT_TRUE(sensor_1_.is_indeterminate());

    test_mast_.loop();
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    EXPECT_EQ(Head_aspect::unknown, protected_head_.get_aspect());
    EXPECT_TRUE(sensor_1_.is_indeterminate());

    protected_head_.request_aspect(Head_aspect::red);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    EXPECT_EQ(Head_aspect::red, protected_head_.get_aspect());

    sensor_1_.set_state(false);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::yellow, head_.get_aspect());
    EXPECT_EQ(Head_aspect::red, protected_head_.get_aspect());

    protected_head_.request_aspect(Head_aspect::yellow);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());

    protected_head_.request_aspect(Head_aspect::green);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());

    sensor_1_.set_state(true);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());
}

TEST(Simple_ryg_test,Protect2SensorsOnly)
{
    Test_head head_;
    Sensor_base sensor_1_;
    Sensor_base sensor_2_;

    Simple_ryg_logic test_mast_(head_,{&sensor_1_,&sensor_2_});


    // While sensor is indeterminate, the head aspect won't change
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    EXPECT_TRUE(sensor_1_.is_indeterminate());
    EXPECT_TRUE(sensor_2_.is_indeterminate());
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());

    sensor_1_.set_state(false);
    sensor_2_.set_state(true);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());


    sensor_1_.set_state(false);
    sensor_2_.set_state(false);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());

    sensor_1_.set_state(true);
    sensor_2_.set_state(false);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());

    sensor_1_.set_state(true);
    sensor_2_.set_state(true);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());

}


TEST(Simple_ryg_test,Protect1SensorsOnly)
{
    Test_head head_;
    Sensor_base sensor_1_;

    Simple_ryg_logic test_mast_(head_,{&sensor_1_});


    // While sensor is indeterminate, the head aspect won't change
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    EXPECT_TRUE(sensor_1_.is_indeterminate());
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());

    sensor_1_.set_state(false);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());


    sensor_1_.set_state(true);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());
}


TEST(Simple_ryg_test,NoProtection)
{
    Test_head head_;
    Simple_ryg_logic test_mast_(head_,{});

    // With just a head attached, it should just go and stay Head_aspect::green
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());

    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());

}


TEST_F(Quadln_s_test,LockedSwitches)
{
    SetUp();

    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());
    EXPECT_EQ(Head_aspect::unknown,head_->get_aspect());

    // After locking switch_1_, so that it fails to set, setting to
    // Head_aspect::green or Head_aspect::red should fail and no switch states should change
    test_switch_1_.set_lock(true);

    EXPECT_FALSE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());

    // Locking the midpoint switch should cause Head_aspect::yellow to fail
    midpoint_switch_.set_lock(true);
    EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());

    // Unlock switch 1, try Head_aspect::red & Head_aspect::green again.  Should still
    // fail because the midpoint is locked, but we'll see switch_1
    // change (its set first in the logic)
    test_switch_1_.set_lock(false);
    EXPECT_FALSE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());


    // Clear the lock so that test_switch_1_ can successfully change
    // and check that the expected states still work
    midpoint_switch_.set_lock(false);
    EXPECT_TRUE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,midpoint_switch_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,midpoint_switch_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,midpoint_switch_.get_direction());
}

TEST_F(Quadln_s_test,SwitchStates)
{
    SetUp();

    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());
    EXPECT_EQ(Head_aspect::unknown,head_->get_aspect());

    // aspect Head_aspect::dark should be rejected by the head and not affect the switches
    EXPECT_FALSE(head_->request_aspect(Head_aspect::dark));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,midpoint_switch_.get_direction());

    // Going to Head_aspect::yellow should not change the value of test_switch
    // ... check after each transition to Head_aspect::yellow
    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,midpoint_switch_.get_direction());

    // Exercise the basic states
    EXPECT_TRUE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,midpoint_switch_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,midpoint_switch_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,midpoint_switch_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,midpoint_switch_.get_direction());
}


TEST_F(Double_switch_test,SwitchStates)
{
    SetUp();

    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());
    EXPECT_EQ(Head_aspect::unknown,head_->get_aspect());

    // After locking switch_1_, so that it fails to set, setting to
    // any of the 4 supported aspects should fail and no switch states
    // should change as switch_1_ is tested first
    test_switch_1_.set_lock(true);

    EXPECT_FALSE(head_->request_aspect(Head_aspect::dark));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    // If switch 2 is locked and switch 1 can set, each
    // supported aspect should still fail but switch_1 should
    // now set
    test_switch_1_.set_lock(false);
    test_switch_2_.set_lock(true);

    EXPECT_FALSE(head_->request_aspect(Head_aspect::dark));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    // Test all of the aspects once the second switch is
    // unlocked
    test_switch_2_.set_lock(false);
    EXPECT_TRUE(head_->request_aspect(Head_aspect::dark));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,test_switch_2_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,test_switch_2_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,test_switch_2_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,test_switch_2_.get_direction());
}


TEST_F(Double_switch_test,HeldStates)
{
    SetUp();

    // Check hold request is rejected if head's aspect is Head_aspect::unknown (default)
    head_->set_held(true);
    EXPECT_FALSE(head_->is_held());

    // Check hold request is accepted if aspect is Head_aspect::red
    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_FALSE(head_->is_held());
    head_->set_held(true);
    EXPECT_TRUE(head_->is_held());

    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,test_switch_2_.get_direction());


    // Head remains held if the same aspect is requestd
    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_TRUE(head_->is_held());

    // Head should reject all changes while held, and still report Head_aspect::red
    EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_FALSE(head_->request_aspect(Head_aspect::green));
    EXPECT_FALSE(head_->request_aspect(Head_aspect::dark));

    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,test_switch_2_.get_direction());


    EXPECT_TRUE(head_->is_held());
    EXPECT_EQ(Head_aspect::red,head_->get_aspect());

    // Setting the held aspect should report success
    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));


    // Clearing the hold should not change the aspect state, and the mast
    // should now accept changes
    head_->set_held(false);
    EXPECT_FALSE(head_->is_held());
    EXPECT_EQ(Head_aspect::red,head_->get_aspect());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Head_aspect::green,head_->get_aspect());
    EXPECT_FALSE(head_->is_held());

}
/*
TEST_F(Single_switch_sensor_test,SwitchStates)
{
    SetUp();

    SCOPED_TRACE("Sensor Head_aspect::unknown");

    // Run these tests twice; once with the sensor indeterminate and
    // once with it set to false
    for(int i=0;i<2;i++) {
        switch_1_.request_direction(Switch_direction::thrown);

        // Head_aspect::yellow & Head_aspect::green should not be accepted while sensor is indeterminate or inactive
        EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
        EXPECT_EQ(Switch_direction::thrown,switch_1_.get_direction());
        EXPECT_FALSE(head_->request_aspect(Head_aspect::green));
        EXPECT_EQ(Switch_direction::thrown,switch_1_.get_direction());

        // Head_aspect::red and Head_aspect::dark should set in all cases
        EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
        EXPECT_EQ(Switch_direction::closed,switch_1_.get_direction());

        switch_1_.request_direction(Switch_direction::thrown);
        EXPECT_TRUE(head_->request_aspect(Head_aspect::dark));
        EXPECT_EQ(Switch_direction::closed,switch_1_.get_direction());

        sensor_1_.set_state(false);

        SCOPED_TRACE("sensor:=false");
    }

    // With the sensor active, all states should be accepted and change
    // the state of the switch
    sensor_1_.set_state(true);

    switch_1_.request_direction(Switch_direction::thrown);
    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::closed,switch_1_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::thrown,switch_1_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::dark));
    EXPECT_EQ(Switch_direction::closed,switch_1_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::thrown,switch_1_.get_direction());
}
*/
TEST_F(Single_switch_test,SwitchStates)
{
    SetUp("");
    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(Head_aspect::unknown,head_->get_aspect());


    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::dark));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
}

TEST_F(Single_switch_test,HeldStates)
{
    SetUp("");
    // Check hold request is rejected if head's aspect is Head_aspect::unknown (default)
    head_->set_held(true);
    EXPECT_FALSE(head_->is_held());

    // Check hold request is accepted if aspect is Head_aspect::red
    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_FALSE(head_->is_held());
    head_->set_held(true);
    EXPECT_TRUE(head_->is_held());

    // Head remains held if the same aspect is requestd
    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_TRUE(head_->is_held());

    // Head should reject all changes while held, and still report Head_aspect::red
    EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_FALSE(head_->request_aspect(Head_aspect::green));
    EXPECT_FALSE(head_->request_aspect(Head_aspect::dark));


    EXPECT_TRUE(head_->is_held());
    EXPECT_EQ(Head_aspect::red,head_->get_aspect());

    // Setting the held aspect should report success
    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));


    // Clearing the hold should not change the aspect state, and the mast
    // should now accept changes
    head_->set_held(false);
    EXPECT_FALSE(head_->is_held());
    EXPECT_EQ(Head_aspect::red,head_->get_aspect());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Head_aspect::green,head_->get_aspect());
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
    EXPECT_STREQ("u",get_aspect_string(Head_aspect::unknown));

    EXPECT_STREQ("d",get_aspect_string(Head_aspect::dark));
    EXPECT_STREQ("r",get_aspect_string(Head_aspect::red));
    EXPECT_STREQ("y",get_aspect_string(Head_aspect::yellow));
    EXPECT_STREQ("g",get_aspect_string(Head_aspect::green));
    EXPECT_STREQ("e",get_aspect_string((Head_aspect)20));  // something invalid
}



TEST_F (Test_switch_test, TestSwitchBehavior){

    EXPECT_EQ(Switch_direction::unknown,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    EXPECT_TRUE(test_switch_1_.request_direction(Switch_direction::thrown));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    EXPECT_TRUE(test_switch_1_.request_direction(Switch_direction::closed));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());

    test_switch_1_.loop();
    EXPECT_EQ(1,test_switch_1_.get_loop_cnt());
}

TEST(InvertedSensor,Inversion)
{
    Sensor_base sensor_;
    Inverted_sensor inverted_sensor_(sensor_);

    EXPECT_TRUE(sensor_.is_indeterminate());
    EXPECT_TRUE(inverted_sensor_.is_indeterminate());

    EXPECT_FALSE(inverted_sensor_.get_state());

    sensor_.set_state(true);
    EXPECT_FALSE(sensor_.is_indeterminate());
    EXPECT_FALSE(inverted_sensor_.is_indeterminate());
    EXPECT_TRUE(sensor_.get_state());
    EXPECT_FALSE(inverted_sensor_.get_state());

    sensor_.set_state(false);
    EXPECT_FALSE(sensor_.is_indeterminate());
    EXPECT_FALSE(inverted_sensor_.is_indeterminate());
    EXPECT_FALSE(sensor_.get_state());
    EXPECT_TRUE(inverted_sensor_.get_state());
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


