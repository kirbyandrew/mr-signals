/*
 * base_tests.cpp
 *
 * Implements unit tests for all of the mr-signals base & interface classes
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */


#include <quadln_switch_head.h>
#include "gtest/gtest.h"


#include "sensor_interface.h"
#include "head_interface.h"
#include "switch_interface.h"
#include "single_switch_head.h"
#include "double_switch_head.h"
#include "logic_collection.h"
#include "ryg_logic.h"
#include "helpers.h"

using namespace mr_signals;


/*
 * Test fixture class to test the QuadLN_S head driver
 *
 * This class simply saves the need to declare the head and switches multiple
 * times between the different test macros
 */
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


/*
 * Test fixture class to test the Double_switch_head
 *
 * This class simply saves the need to declare the head and switches multiple
 * times between the different test macros
 */
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
 * Test fixture class to test the Single_switch_head driver
 *
 * This class saves the need to declare the head and switch multiple
 * times, and allows the test macro to pass different names to test
 * the name functionality of Head_interface::
 */


class Single_switch_head_test : public ::testing::Test {

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



/*
 * Test the basic function of the Red_head_sensor, which returns a sensor
 * state based on the aspect of an associated head.
 *
 * The Sensor should return inactive for all aspects of the associated head,
 * except red, where it should return active
 *
 * The sensor should not be indeterminate at all times
 */
TEST(Red_head_sensor_test,BasicStates)
{
    Test_head head;
    Red_head_sensor sensor(head);

    EXPECT_FALSE(sensor.is_indeterminate());

    // Loop through all aspects; the Red_head_sensor should return inactive for all
    // aspects other than ::red
    for(Head_aspect aspect=Head_aspect::unknown; aspect<Head_aspect::max_head_aspect;aspect++) {
        head.request_aspect(aspect);

        EXPECT_FALSE(sensor.is_indeterminate());

        if(Head_aspect::red == aspect){
            EXPECT_TRUE(sensor.is_active());
        }
        else {
            EXPECT_FALSE(sensor.is_active());
        }
    }

}

/*
 * Test the basic function of a Lever_with_pushkey sensor
 *
 * The combined sensor's state should be indeterminate until the state of both
 * the associated level and push key are known.
 *
 * The combined sensor should only return a state of true if the lever is
 * reversed (goes active) when the pushkey is pressed (active), otherwise
 * it should return false.
 *
 * The combined sensor will then stay active if the pushkey is released
 *
 * Each call to is_active() is repeated to simulate the repeated calling
 * of the function on a target to ensure there are no latching logic errors
 *
 */

TEST(Lever_with_pushkey_test,BasicStates)
{
    Test_sensor lever_;
    Test_sensor push_key_;
    Lever_with_pushkey interlocked_lever_(lever_,push_key_);

    // Check initial conditions
    EXPECT_FALSE(interlocked_lever_.is_active());

    EXPECT_TRUE(interlocked_lever_.is_indeterminate());
    lever_.set_state(false);
    EXPECT_TRUE(interlocked_lever_.is_indeterminate());
    push_key_.set_state(false);
    EXPECT_FALSE(interlocked_lever_.is_indeterminate());

    // Interlock lever should remain inactive when the lever is thrown
    // and the pushkey is not pressed
    lever_.set_state(true);
    EXPECT_FALSE(interlocked_lever_.is_active());
    EXPECT_FALSE(interlocked_lever_.is_active());

    // Pushing the key after throwing the lever does not set the Interlocked
    // lever to active
    push_key_.set_state(true);
    EXPECT_FALSE(interlocked_lever_.is_active());
    EXPECT_FALSE(interlocked_lever_.is_active());

    // Set the lever to Normal, leaving the pushkey down, then reverse
    // the lever again, Interlocked lever should go active
    lever_.set_state(false);
    EXPECT_FALSE(interlocked_lever_.is_active());
    EXPECT_FALSE(interlocked_lever_.is_active());

    lever_.set_state(true);
    EXPECT_TRUE(interlocked_lever_.is_active());
    EXPECT_TRUE(interlocked_lever_.is_active());

    // Should stay active if the pushkey is then released
    push_key_.set_state(false);
    EXPECT_TRUE(interlocked_lever_.is_active());
    EXPECT_TRUE(interlocked_lever_.is_active());
}

/*
 * Test an interlocking lever with a pushkey controlling a Call-On signal head
 *
 * Assuming that any protected sensors are inactive, the head should only clear
 * (non-red) if the pushkey is pressed before the lever is reversed
 */

TEST(Interlocked_ryg_test,CallOnButton)
{
    Test_sensor lever_(true);       // Lever controlling the head
    Test_sensor push_key_(false);   // Push key for the interlocked head
    Test_sensor sensor_1_(false);   // Sensor the head is protecting

    Test_head head_;                // Head under test

    // Combine the lever and the pushkey into a lever object for the logic
    Lever_with_pushkey interlocked_lever_(lever_,push_key_);

    // Interlocked lever logic
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


    // If the lever is then reversed, the head should return to red
    lever_.set_state(false);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());

    // Reversing the lever without the pushkey will leave the head at red
    lever_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());

    // Pressing the key after the lever is reversed will not clear the signal
    push_key_.set_state(true);
    test_logic_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());
}

/*
 * Test the function of a head controlled by an interlocking lever and
 * an 'automated' lever that turns an interlocked head into a standard
 * APB head when reversed, using the Interlocked_ryg_logic
 *
 *
 * In other words, instead of falling and staying at Aspect red
 * if a protected sensor goes active (even if the interlocking lever remains
 * reversed), the head will follow a the next head it is protecting once the
 * sensor returns to inactive if both the interlocking and automated levers
 * remain reversed.
 *
 */

TEST(Interlocked_ryg_logic_test, AutomatedLever)
{
    Test_head head_;                        // The head under test
    Test_head protected_head_;              // Subsequent head that the head is protecting
    Test_sensor sensor_1_(false);           // Sensor the head is protecting
    Test_sensor lever_(false);              // Interlocking lever
    Test_sensor automated_lever_(false);    // Automated behaviour lever

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
    // automated lever reversed
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

/*
 * Test the basic functionality of a Head that's interlocked with a lever
 * using the Interlocked_ryg_logic
 *
 * The lever must be reversed for the head to clear beyond a red aspect.
 * With the lever reversed, the head acts as a standard ryg head. However
 * if a protected sensor goes active, the head will fall again to red
 * and latch there until the lever is set to normal and reversed again
 * even as the sensor stays inactive.
 */

TEST(Interlocked_ryg_logic_test, BasicInterlocking)
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

/*
 * Test the ability of the Simple_ryg_logic to appropriately protect a head
 * and a sensor.
 *
 * If the sensor is active:
 *   The head will be red
 * If the sensor is inactive:
 *   If the protected head is red, the head will be yellow
 *   If the protected head is yellow or green, the head will be green
 */

TEST(Simple_ryg_logic_test, ProtectHeadAndSensor)
{
    Test_head head_;                // Head the protects the following head and sensor
    Test_head protected_head_;      // The head that is protected by head_
    Sensor_base sensor_1_;          // The sensor that is protected by head_

    Simple_ryg_logic test_mast_(head_,protected_head_,{&sensor_1_});

    // Test defaults
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    EXPECT_EQ(Head_aspect::unknown, protected_head_.get_aspect());
    EXPECT_TRUE(sensor_1_.is_indeterminate());

    test_mast_.loop();
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    EXPECT_EQ(Head_aspect::unknown, protected_head_.get_aspect());
    EXPECT_TRUE(sensor_1_.is_indeterminate());

    // If the protected head goes red, but the sensor is still
    // indeterminate, the head remains unknown
    protected_head_.request_aspect(Head_aspect::red);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::unknown, head_.get_aspect());
    EXPECT_EQ(Head_aspect::red, protected_head_.get_aspect());

    // If the sensor becomes inactive, the head should fall
    // to yellow as the protected head is red
    sensor_1_.set_state(false);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::yellow, head_.get_aspect());
    EXPECT_EQ(Head_aspect::red, protected_head_.get_aspect());

    // When the protected head goes to yellow or red, the head will go green
    protected_head_.request_aspect(Head_aspect::yellow);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());

    protected_head_.request_aspect(Head_aspect::green);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());

    // If the sensor goes active, the head should go to red
    sensor_1_.set_state(true);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::red, head_.get_aspect());
}

/*
 * Test the OR logic of the protected sensors for Simple_ryg_logic.
 *
 * If either of two protected sensors are active, the head should go red,
 * otherwise it should be green.
 */
TEST(Simple_ryg_logic_test, Protect2SensorsOnly)
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

    // Test the 4 combinations of the 2 sensor states
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

/*
 * Test that a single protected sensor being active will set a head
 * red using the Simple_ryg_logic
 */
TEST(Simple_ryg_logic_test, Protect1SensorsOnly)
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

    sensor_1_.set_state(false);
    test_mast_.loop();
    EXPECT_EQ(Head_aspect::green, head_.get_aspect());
}

/*
 * Test the behaviour of Simple_ryg_logic with only a head
 * attached; the head should go and stay green with no
 * protected sensors or heads.
 */

TEST(Interlocked_ryg_logic_test, NoProtection)
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

/*
 * Test the Quadln logic when a switch fails to change state
 *
 * In the red and green cases, if the 'first' switch's state is locked,
 * attempts to set it's value will fail, and calls to request_aspect()
 * should also fail.  Additionally, the 'lock' switch's state should
 * not change if the Quadln_s_head cannot set the first switch's state.
 *
 * If the 'lock' switch's state is locked, but the 'first' switch is not,
 * attempts to set the aspect should still fail but the state of the
 * 'first' switch should change accordingly.
 */

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

    // And a final check to ensure that it's only the state of the midpoint switch
    // that changes when yellow is requested
    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,midpoint_switch_.get_direction());

}

/*
 * Test the order of the logic in Quadln_s_head when attempting to set a
 * given aspect.
 *
 */

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

    // Check that the loop function calls the loop of each switch
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());
    EXPECT_EQ(0,midpoint_switch_.get_loop_cnt());
    head_ -> loop();
    EXPECT_EQ(1,test_switch_1_.get_loop_cnt());
    EXPECT_EQ(1,midpoint_switch_.get_loop_cnt());

}

/*
 * Test the general behavior of the Double_switch_head, which controls
 * two switches.
 *
 * Check the behavior when the first switch fails to change state (change of
 * the head's state should fail, neither of the switches state changes)
 *
 * Check the behavior when the second switch fails to change state (change of
 * the head's state should fail, but the state of the first switch will change)
 *
 * Exercise all of the states of the two switches for the aspects supported by
 * the head
 */


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
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    EXPECT_FALSE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::unknown,test_switch_2_.get_direction());

    // Test all of the aspects once the second switch is
    // unlocked
    test_switch_2_.set_lock(false);
    EXPECT_TRUE(head_->request_aspect(Head_aspect::dark));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,test_switch_2_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::green));
    EXPECT_EQ(Switch_direction::closed,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,test_switch_2_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::thrown,test_switch_2_.get_direction());

    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,test_switch_2_.get_direction());

    // Check that the loop function calls the loop of each switch
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());
    EXPECT_EQ(0,test_switch_2_.get_loop_cnt());
    head_ -> loop();
    EXPECT_EQ(1,test_switch_1_.get_loop_cnt());
    EXPECT_EQ(1,test_switch_2_.get_loop_cnt());

}

/*
 * Test the behavior of the Double_switch_head in cases where the head
 * is held.
 */

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

    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,test_switch_2_.get_direction());


    // Head remains held if the same aspect is requestd
    EXPECT_TRUE(head_->request_aspect(Head_aspect::red));
    EXPECT_TRUE(head_->is_held());

    // Head should reject all changes while held, and still report Head_aspect::red
    EXPECT_FALSE(head_->request_aspect(Head_aspect::yellow));
    EXPECT_FALSE(head_->request_aspect(Head_aspect::green));
    EXPECT_FALSE(head_->request_aspect(Head_aspect::dark));

    EXPECT_EQ(Switch_direction::thrown,test_switch_1_.get_direction());
    EXPECT_EQ(Switch_direction::closed,test_switch_2_.get_direction());


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
 * Test that the states of the switch associated with a Single_switch_head
 * set as expected for the supported aspects.
 */
TEST_F(Single_switch_head_test,SwitchStates)
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


    // Check that the loop function calls the loop of the switch
    EXPECT_EQ(0,test_switch_1_.get_loop_cnt());
    head_ -> loop();
    EXPECT_EQ(1,test_switch_1_.get_loop_cnt());
}


/*
 * Test the 'held' functionality of a head.  When a head is held,
 * its aspect should not change, and requests to change it are
 * discarded until the hold is removed, after which requests to
 * change it's aspect should be honored.
 */
TEST_F(Single_switch_head_test,HeldStates)
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

    // Head remains held if the same aspect is requested
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

/*
 * Confirm that names longer than head_name_len characters are truncated by
 * a head (using Single_switch_head)
 */

TEST_F(Single_switch_head_test,HeadLongNameCheck)
{
    SetUp("Head1_");
    EXPECT_STREQ("Head1",head_->get_name());        // Expect truncated
}


/*
 * Confirm that names exactly head_name_len characters is correctly handled
 * by a head (using Single_switch_head)
 */

TEST_F(Single_switch_head_test,HeadExactNameCheck)
{
    const char name[] = "Head1";
    SetUp(name);
    EXPECT_STREQ(name,head_->get_name());        // Expect exact
}


/*
 * Confirm that names shorter than head_name_len characters are supported
 * by a head (using Single_switch_head)
 */

TEST_F(Single_switch_head_test,HeadShortNameCheck)
{
    SetUp("a");
    EXPECT_STREQ("a",head_->get_name());        // Expect truncated
}

/*
 * Confirm that an empty name passed to a head returns a correctly
 * terminated string (using Single_switch_head)
 */


TEST_F(Single_switch_head_test,EmptyNameCheck)
{
    SetUp("");
    const char* name = head_->get_name();
    EXPECT_LT(strlen(name),5u);        // Expect terminated string of head_name_len or less
}

/*
 * Confirm the expected head aspect abbreviations returned by
 * get_aspect_string() for each valid and an invalid Head_aspect::
 * value
 */

TEST(AspectNameStringTest, gets)
{
    EXPECT_STREQ("u",get_aspect_string(Head_aspect::unknown));

    EXPECT_STREQ("d",get_aspect_string(Head_aspect::dark));
    EXPECT_STREQ("r",get_aspect_string(Head_aspect::red));
    EXPECT_STREQ("y",get_aspect_string(Head_aspect::yellow));
    EXPECT_STREQ("g",get_aspect_string(Head_aspect::green));
    EXPECT_STREQ("e",get_aspect_string((Head_aspect)20));  // something invalid
}


/*
 * Test the behavior of the Test_switch class
 *
 * The test switch should start with an unknown direction, accept requests
 * to change direction to thrown and closed, and only increment
 * its loop count when the loop() method is called
 */

TEST (Test_switch_test, TestSwitchBehavior)
{
    Test_switch test_switch_1_;

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

/*
 * Test the function of an Inverted_sensor.
 *
 * When the underlying sensor is indeterminate, the inverted sensor should
 * also be indeterminate AND false
 *
 * Once the underlying sensor state is set, the inverted sensor should
 * not return indeterminate, and the opposite state of the underlying one
 *
 */

TEST(InvertedSensor,Inversion)
{
    Sensor_base sensor_;
    Inverted_sensor inverted_sensor_(sensor_);

    EXPECT_TRUE(sensor_.is_indeterminate());
    EXPECT_TRUE(inverted_sensor_.is_indeterminate());

    EXPECT_FALSE(inverted_sensor_.is_active());

    sensor_.set_state(true);
    EXPECT_FALSE(sensor_.is_indeterminate());
    EXPECT_FALSE(inverted_sensor_.is_indeterminate());
    EXPECT_TRUE(sensor_.is_active());
    EXPECT_FALSE(inverted_sensor_.is_active());

    sensor_.set_state(false);
    EXPECT_FALSE(sensor_.is_indeterminate());
    EXPECT_FALSE(inverted_sensor_.is_indeterminate());
    EXPECT_FALSE(sensor_.is_active());
    EXPECT_TRUE(inverted_sensor_.is_active());
}

/*
 * Helper function for testing whether a sensor's state actually changes when
 * a state (true/false) is requested of it
 *
 * &sensor: The sensor to test
 * state : The state to request of the sensor
 * change_expected: Indicates whether the state of the sensor is expected to change with
 *                  the request
 */
void sensor_base_set_state_test(Sensor_base& sensor, bool state,
        bool change_expected)
{

    // EXPECT_EQ does not appear to evaluate true particularly consistently; looks like
    // it is cast to int internally and different non-0 values *sometimes* get returned
    // and the test will fail so we cannot use
    //
    // EXPECT_EQ(change_expected, sensor.set_state(state));

    if(change_expected){
        EXPECT_TRUE(sensor.set_state(state));
    }
    else {
        EXPECT_FALSE(sensor.set_state(state));
    }


    EXPECT_EQ(false, sensor.is_indeterminate());
    EXPECT_EQ(state , sensor.is_active());
}


/*
 * Test the initialization and then changing of states of the base sensor
 *
 * Use two objects to test different order (true-false-true vs false-true-false)
 * from an uninitialized state
 *
 */
TEST(Base_sensor_test, TestStates)
{
    Sensor_base base_sensor_1_;
    Sensor_base base_sensor_2_;

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


/*
 * Test the behavior of the Active_sensor class
 *
 * The sensor is never indeterminate and always returns an active state
 *
 * Also, the Active_sensor should be derived from Sensor_Interface, not
 * Sensor_base, so we should not be able to cast to Sensor_base and access
 * set_state()
 */

TEST(Active_sensor_test, IsEmptyInitially) {

  Active_sensor active_sensor_;

  EXPECT_EQ(false, active_sensor_.is_indeterminate());
  EXPECT_EQ(true, active_sensor_.is_active());

  Sensor_interface* sensor_interface_ = &active_sensor_;
  Sensor_base* sensor_base_ = dynamic_cast<Sensor_base*>(sensor_interface_);

  EXPECT_EQ(NULL,sensor_base_);

}


