/*
 * sar_mast_tests.cpp
 *
 *  Created on: Feb 22, 2018
 *      Author: ackpu
 */

#include <iostream>
#include "gtest/gtest.h"

#include "sensor_interface.h"
#include "head_interface.h"
#include "switch_interface.h"
//#include "single_switch_head.h"
//#include "double_switch_head.h"
#include "quadln_s_head.h"
#include "logic_collection.h"
#include "ryg_logic.h"
#include "helpers.h"

using namespace mr_signals;

// copy vector to an array
// std::copy(source.begin(),source.end(),destination)


/*
 * struct A { double x, double y};
 * struct B { vector<double> values; }
 * struct C {A a; B b; }
 *
 * C c = {{0.3. 0.01},{{14.2,18.1,0.0,3.2}}};
 *
 * C obj = { {0.0,0.1}, std::vector<double>(2,0.0) };
 *
 * double values[] = {1,2,3,4,5};
 * C c = {{1,0.1}, {std::vector<double>(values, values+5)}};
 *
 * vector containing 10 vectors of 5
 * std::vector<std::vector<int>> v(10,std::vector<int>(5));
 *
 * std::array<std::vector<int>,10> v{{  std::vector<int>(5),
 *                                      std::vector<int>(5),
 *                                      ...
 *
 *
 *  std::array<std::vector<int>,10> v;
 *  std::fill(v.begin(),v.end(), std::vector<int>(5));
 */

enum Sar_speed_signal_names
{
        unused=0,
        stop_signal,
        caution_normal_speed,
        clear_normal_speed,
        caution_medium_speed,
        clear_medium_speed,
        reduce_to_medium_speed,
        caution_low_speed

};

struct Mast_configuration
{
    int name;
    std::vector<Head_aspect> head_aspects;
    int preceding_name;
};

//Mast_aspect sar_speed_aspects[] = {
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

class Mast_aspects_container
{
public:
    /*
    Mast_aspects_container(
            std::initializer_list<Mast_aspect> const & mast_rules) :
            mast_rules_(mast_rules)
    {
    }
    */
    /// Construct the object by specifying the maximum number of
    /// heads that the masts can hold (so that masts with fewer
    /// than the maximum can be dummied to ensure 1-to-1 matches
    /// with the aspect combinations specified in the mast_rules_
    Mast_aspects_container(const unsigned int max_heads,
            std::vector<Mast_configuration> const & mast_rules) :
            mast_rules_(mast_rules), max_heads_(max_heads) {
    }

    /// Add masts in the reverse order of travel; that is add the
    /// last mast in the direction of travel, followed by its
    /// preceding mast and so on
    void add_mast(std::initializer_list<Head_interface*> const & mast)
    {
        std::vector<Head_interface*> mast_temp;

        mast_temp = mast;

        while(mast_temp.size() < max_heads_) {
            mast_temp.push_back(&fixed_red_head_);
        }

        masts_.push_back(mast_temp);
    }


    // Get the name of the mast's aspect as defined by a vector of Head_aspect::s
    int get_mast_name(const std::vector<Head_aspect> head_aspects)
    {
        for(auto &rule : mast_rules_ ) {
            if(head_aspects == rule.head_aspects) {
                return rule.name;
            }
        }
        // No matches found, return error
        return 0;
    }

    int get_mast_name(const unsigned int mast_idx)
    {
        if(mast_idx < masts_.size()) {
            std::vector<Head_aspect> aspects;
            for(auto &head : masts_[mast_idx]) {
                Head_aspect aspect = head->get_aspect();
                aspects.push_back(aspect);
            }

            return get_mast_name(aspects);
        }
        return 0;
    }

    // Get the preceding name corresponding to a given name in the mast rules
    int get_preceding_mast_name(const int name) {

        for(auto& rule : mast_rules_) {
            if(name == rule.name) {
                return rule.preceding_name;
            }
        }

        return 0;
    }

    // Check all masts follow the preceding name as specified in the
    // rules, with the name of the first mast being passed.
    // The sequence can start after the first mast by specifying the start index;
    // by default the sequence starts at the first mast in the list
    bool check_mast_sequence(const int start_name, unsigned int mast_idx=0)
    {
        if(mast_idx < masts_.size()) {

            int preceding_name = start_name;

            for(unsigned int mast=mast_idx;mast < masts_.size();mast++) {

                if(preceding_name != get_mast_name(mast)) {
                    return false;
                }
                preceding_name = get_preceding_mast_name(preceding_name);
            }

            // Looped through all masts successfully
            return true;
        }

        // Default error case
        return false;
    }

    // Check the names of the current mast aspects against a list of names
    // that are provided
    bool check_mast_sequence(const std::vector<int> names)
    {
        return false;
    }

    void dump_mast_states()
    {

        for(auto& mast: masts_) {
            std::vector<Head_aspect> aspects;
            for(auto &head : mast) {
                std::cout << head->get_name() << " Aspect : " << get_aspect_string(head->get_aspect()) << "\n";
                aspects.push_back(head->get_aspect());
            }
            std::cout << "Mast name : " << get_mast_name(aspects) << "\n";
        }

        std::cout << "\n";

    }



private:

    // The rules this class uses
    std::vector<Mast_configuration> const mast_rules_;

    // The heads that are defined for this class to evaluate
    std::vector<std::vector<Head_interface*>>  masts_;

    // Maximum number of heads per mast
    unsigned int max_heads_;

    // Local object of a fixed red head to put into masts that are defined with
    // less than the maximum number of heads
    Fixed_red_head fixed_red_head_;

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

    Simple_ryg_logic l_1107a(h_1107a,h_1a,{&t_1107});
    Simple_ryg_logic l_1107b(h_1107b,h_1b,{&t_1107, &h_1b_red});


    Interlocked_ryg_logic l_1a(h_1a,h_22a,lever_1, auto_lever, {&t_1,&switch_6});
    Interlocked_ryg_logic l_1b(h_1b,h_red,lever_1, {&t_1,&switch_6_inv,&switch_8});
    Interlocked_ryg_logic l_1c(h_1c,lever_1c,  {&t_1,&switch_6});

    Interlocked_ryg_logic l_22a(h_22a,h_red, lever_22, auto_lever,{&t_22});


    Logic_collection logic_collection({&l_1a,&l_1b,&l_1c,&l_22a,&l_1107a,&l_1107b});

    Mast_aspects_container mast_container(3, sar_speed_masts);

    mast_container.add_mast({&h_1a,&h_1b, &h_1c});
    mast_container.add_mast({&h_1107a,&h_1107b});


    logic_collection.loop();

    mast_container.dump_mast_states();

    EXPECT_EQ(stop_signal, mast_container.get_mast_name({Head_aspect::red, Head_aspect::red, Head_aspect::red}));


    EXPECT_EQ(clear_normal_speed,  mast_container.get_mast_name(0));
    EXPECT_EQ(clear_normal_speed,  mast_container.get_mast_name(1));

    EXPECT_TRUE(mast_container.check_mast_sequence(clear_normal_speed));


    switch_6.set_state(true);

    logic_collection.loop();

    mast_container.dump_mast_states();

    EXPECT_TRUE(mast_container.check_mast_sequence(stop_signal));

    lever_1.set_state(false);
    logic_collection.loop();
    lever_1.set_state(true);
    logic_collection.loop();

    EXPECT_TRUE(mast_container.check_mast_sequence(caution_medium_speed));
    mast_container.dump_mast_states();

}
