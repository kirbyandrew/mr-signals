/*
 * sar_mast_tests.cpp
 *
 * Implements the helper classes and gtest macros to test the construction of
 * signal logic that is compliant with the aspect names and behaviour of the
 * upper Quadrant semaphore signals used by the South Australian Railways
 *
 *  Created on: Feb 22, 2018
 *      Author: ackpu
 */

#include <quadln_s_head.h>
#include <iostream>
#include "gtest/gtest.h"

#include "sensor_interface.h"
#include "head_interface.h"
#include "switch_interface.h"
#include "logic_collection.h"
#include "ryg_logic.h"
#include "helpers.h"

#include "mast_test_helpers.h"

using namespace mr_signals;

/**
 * Create an enumeration of all of the signal names used.
 * The order in the enum is unimportant.
 */

enum Sar_speed_signal_names
{
        unused=0,              //!< unused
        stop_signal,           //!< stop_signal
        caution_normal_speed,  //!< caution_normal_speed
        clear_normal_speed,    //!< clear_normal_speed
        caution_medium_speed,  //!< caution_medium_speed
        clear_medium_speed,    //!< clear_medium_speed
        reduce_to_medium_speed,//!< reduce_to_medium_speed
        caution_low_speed      //!< caution_low_speed

};


std::vector<Mast_configuration> sar_speed_masts = {
    {   stop_signal,
            {{Head_aspect::red, Head_aspect::red, Head_aspect::red}},
            caution_normal_speed },
    { caution_normal_speed,
            {{Head_aspect::yellow, Head_aspect::red, Head_aspect::red}},
            clear_normal_speed},
    {   clear_normal_speed,
            {{Head_aspect::green, Head_aspect::red, Head_aspect::red}},
            clear_normal_speed},
    {   caution_medium_speed,
            {{Head_aspect::red, Head_aspect::yellow, Head_aspect::red}},
            reduce_to_medium_speed},
    {   clear_medium_speed,
            {{Head_aspect::red, Head_aspect::green, Head_aspect::red}},
            reduce_to_medium_speed},
    {   caution_low_speed,
            {{Head_aspect::red, Head_aspect::red, Head_aspect::yellow}},
            caution_normal_speed},
    {   reduce_to_medium_speed,
            { { Head_aspect::yellow, Head_aspect::green, Head_aspect::red }},
            clear_normal_speed},
};


TEST(SarSpeedSignal,BasicStates)
{
    Test_sensor lever_1(true);      // Controls Mast 1
    Test_sensor switch_6(false);    // Down Main -> Up Main (med speed)
    Inverted_sensor switch_6_inv(switch_6);
    Test_sensor switch_8(false);    // Up Main -> Siding (low speed)
    Inverted_sensor switch_8_inv(switch_8);
    Test_sensor lever_22(true);     // Controls Mast 22
    Test_sensor auto_lever(true);   // Automated lever
    Test_sensor push_key_1(false);



    Lever_with_pushkey lever_1c(lever_1,push_key_1);


    Test_sensor t_1107(false);
    Test_sensor t_1(false);
    Test_sensor t_22(false);

    Test_head h_1107a("1107A");
    Test_head h_1107b("1107B");
    Test_head h_1a("bw1A");
    Test_head h_1b("bw1B");
    Test_head h_1c("bw1C");
    Test_head h_22a("bw22a");
    Fixed_red_head h_red;
    Red_head_sensor h_1b_red(h_1b);

    Logic_collection logic_collection(6);

    Simple_ryg_logic l_1107a(logic_collection,h_1107a,h_1a,{&t_1107});
    Simple_ryg_logic l_1107b(logic_collection,h_1107b,h_1b,{&t_1107, &h_1b_red});


    Interlocked_ryg_logic l_1a(logic_collection,h_1a,h_22a,lever_1, auto_lever, {&t_1,&switch_6});
    Interlocked_ryg_logic l_1b(logic_collection,h_1b,h_red,lever_1, {&t_1,&switch_6_inv,&switch_8});
    Interlocked_ryg_logic l_1c(logic_collection,h_1c,lever_1c,  {&t_1,&switch_6});

    Interlocked_ryg_logic l_22a(logic_collection,h_22a,h_red, lever_22, auto_lever,{&t_22});



    Mast_aspects_container mast_container(sar_speed_masts);

    EXPECT_TRUE(mast_container.check_configuration());

    mast_container.add_mast({&h_1a,&h_1b, &h_1c});
    mast_container.add_mast({&h_1107a,&h_1107b});


    logic_collection.loop();
    logic_collection.loop();

    mast_container.dump_mast_states();

    EXPECT_EQ(stop_signal, mast_container.get_mast_name({Head_aspect::red, Head_aspect::red, Head_aspect::red}));


    EXPECT_EQ(clear_normal_speed,  mast_container.get_mast_name(0));
    EXPECT_EQ(clear_normal_speed,  mast_container.get_mast_name(1));

    EXPECT_TRUE(mast_container.check_mast_sequence(clear_normal_speed));


    switch_6.set_state(true);

    logic_collection.loop();
    logic_collection.loop();


    mast_container.dump_mast_states();

    EXPECT_TRUE(mast_container.check_mast_sequence(stop_signal));

    lever_1.set_state(false);
    logic_collection.loop();
    logic_collection.loop();

    lever_1.set_state(true);
    logic_collection.loop();
    logic_collection.loop();


    EXPECT_TRUE(mast_container.check_mast_sequence(caution_medium_speed));
    mast_container.dump_mast_states();

}
