/*
 * mast_tests.h
 *
 * Structures and classes to support writing tests for a specific rulebook's
 * signal names and aspects
 *
 * If individual heads are used in an MR-Signals configuration, it is
 * convenient to be able to test their configurations against an expected
 * mast name (which is a combination of individual head aspects).
 *
 * This class provides the functions to test mast configurations, as well
 * as a model that could be used to build a mast class that combines
 * a number of heads into a single entity if the logic to control them
 * is more complex than can be realized with the the head structure.
 *
 *  Created on: Apr 28, 2018
 *      Author: ackpu
 */

#ifndef TEST_MAST_TEST_HELPERS_H_
#define TEST_MAST_TEST_HELPERS_H_

namespace mr_signals
{


/**
 * Structure to hold the name if a signal mast, the combination of head aspects
 * that combine to create a single aspect associated with the name, and the
 * expected preceding mast's name
 *
 * Each entry in the table must have the same number of aspects defined; the
 * test class expects this to simplify its logic.  Populate unused head aspects
 * with ::red, or override the test class to handle.
 *
 */
struct Mast_configuration
{

    /// Name of the mast, e.g. stop_signal.
    /// Use int to allow any enum to be used here
    int name;

    /// Vector of the head aspects for the name, e.g.
    /// { Head_aspect::yellow, Head_aspect::green, Head_aspect::red }
    std::vector<Head_aspect> head_aspects;

    /// Name of the expected preceeding mast's name
    int preceding_name;
};


/**
 * Helper class to test individual heads in combination to create the overall
 * aspect of a multi-head signal mast.
 *
 * A configuration containing the mast name, corresponding head aspects and
 * the expected preceding signal's name is passed to construct the object.
 *
 * The sequence of masts, comprised of a number of heads, is loaded to create
 * a sequence of masts whose interaction can then be tested.  Multiple sequences,
 * can be created using different instances; these can share the same head if
 * desired
 *
 * Methods are then provided to simplify writing tests to confirm that the
 * signal aspects then behave as expected.
 *
 * For example, using a basic green -> yellow -> red signal relationship,
 * if the first mast tested is expected to be at stop (red), the next would
 * be expected to be at caution (yellow), finally followed by clear (green).
 * The more common use case however is to test multi-head masts, which aspect
 * combinations are more complex than the simple green/yellow/red.
 *
 */

class Mast_aspects_container
{
public:

    /**
     * Construct an object by specifying the maximum number of
     * heads that the masts can hold (so that masts with fewer
     * than the maximum can be dummied to ensure 1-to-1 matches
     * with the aspect combinations specified in the mast_rules_
     *
     * @param mast_rules  The mast rules to initialize the container with
     */


    // TODO: What is the correct way to work around const correctness?
    // Why can't this be passed to check_configuration()?
    Mast_aspects_container(std::vector<Mast_configuration> const & mast_rules) :
            mast_rules_(mast_rules),max_heads_(0) {
    }

    /**
     * Check the configuration.
     *
     * Separated from the constructor to allow the result to be checked in a
     * test sequence
     *
     * @return True if the passed rules have 1 or more configurations each of which
     * has the same number of Head_aspects defined
     */
    bool check_configuration(void) {

        if(mast_rules_.size() > 0)
        {
            max_heads_ = mast_rules_.at(0).head_aspects.size();

            for(auto &rule : mast_rules_ ) {
                if(rule.head_aspects.size() != max_heads_) {
                    return false;
                }
            }

           return true;
        }
        return false;
    }


    /**
     *  Add a signal mast, composed of a number of heads.
     *
     *  Add masts in the reverse order of 'travel', e.g. to test signal masts
     *  1, 2 & 3 who are passed in that order, load masts 3, 2 & 1
     *
     *  If fewer heads are defined for this mast than the number defined
     *  in the configuration (a common real-world scenario), fixed reds are
     *  appended to match the expected value from the configuration.  Override
     *  this if other aspects are used.
     *
     * @param mast  List of Head_interface or derived classes
     */

    void add_mast(std::initializer_list<Head_interface*> const & mast)
    {
        // Create a local vector so that fixed red heads can be appended
        // if fewer than the maximum heads are passed
        //
        // If more than the maximum are passed, they'll be ignored
        std::vector<Head_interface*> mast_temp;

        mast_temp = mast;

        while(mast_temp.size() < max_heads_) {
            mast_temp.push_back(&fixed_red_head_);
        }

        masts_.push_back(mast_temp);

    }


    /// Get the name (enumerated value) of the mast's aspect as defined by a vector of Head_aspect::s
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

    /// Get a mast's name based on an index of the loaded masts
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

    /// Get the preceding name corresponding to a given name in the mast rules
    int get_preceding_mast_name(const int name) {

        for(auto& rule : mast_rules_) {
            if(name == rule.name) {
                return rule.preceding_name;
            }
        }

        return 0;
    }

    /**
     * Check that a range of loaded is matching the specified name
     * relationships in the configuration, starting from the first
     * expected mast name.
     * The sequence can start after the first mast by specifying the start index;
     * by default the sequence starts at the first mast in the list
     *
     * For example, using a basic green -> yellow -> red signal relationship,
     * the first mast tested would be expected to be at stop (red), with the next
     * at caution (yellow), finally followed by clear (green)
     *
     * @param start_name - the expected name of the first tested mast
     * @param mast_idx   - optional parameter to start the check from some
     *                     other point than the first on the list.
     * @return true if the first mast matches the start name
     */
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

    /**
     * Print out the aspects of the loaded masts (typically used for
     * debugging unexpected behavior)
     */
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

    // The rules this class uses; passed at construction of the object
    std::vector<Mast_configuration> const mast_rules_;

    // The heads that are defined for this class to evaluate
    std::vector<std::vector<Head_interface*>>  masts_;

    // Maximum number of heads per mast
    unsigned int max_heads_;

    // Local instance of a fixed red head to put into masts that are defined with
    // less than the maximum number of heads
    Fixed_red_head fixed_red_head_;

};

// TODO: Update entire file into just helpers; not just mast_test_helpers.  Add needed #includes.

/* Enum to support class Logic_sensor_test() and similar */
enum class Sensor_set { inactive, active, nochange };
enum class Sensor_test{ inactive,  active,  indeterminate} ;

/**
 * Helper class to simplify writing tests for interactions of sensors
 *
 * Concrete instance is declared with a Logic_interface to loop, a list of
 * 'input' sensors whose state is set as part of the test action,
 *  and a list of 'output' sensors whose state is then tested after the
 *  logic's loop function is called
 *
 * Example:
 * Logic_interface my_logic;
 * Sensor_base in_sensor1;
 * Sensor_base in_sensor2;
 * Sensor_base out_sensor1;
 * Sensor_base out_sensor2;
 *
 * Logic_sensor_test sensor_test(my_logic,
 *                               {in_sensor1, in_sensor2},
 *                               {out_sensor1, out_sensor2});
 *
 * # If input sensors are set to true and not changed respectively,
 * # expect indeterminate and false in the outputs after running the logic
 * # loop
 * EXPECT_TRUE(sensor_test.set_loop_test({1,-1},{-1,0}))
 *
 * this would set in_sensor1 active, not change in_sensor2, and expects
 * out_sensor1 to be indeterminate, and out_sensor2 to be inactive.
 *
 * This is the expected relationship
 *
 */


class Logic_sensor_test {

public:
    Logic_sensor_test(  Logic_interface &logic,
                        std::initializer_list<Sensor_base *> const & sensors_to_set,
                        std::initializer_list<Sensor_interface*> const & sensors_to_test) :
                            logic_(logic),
                            sensors_to_set_(sensors_to_set),
                            sensors_to_test_(sensors_to_test) {
    }

#if 0
    bool set_loop_test( std::initializer_list<Sensor_set> set_sensor_values,
                        std::initializer_list<Sensor_test> test_sensor_values) {

        // std::vector<Sensor_set> set_sensor_values_(set_sensor_values);
        // std::vector<Sensor_test> test_sensor_value_(test_sensor_values);

        // std::vector<testNetInfo> netInfo(initList.begin(), initList.end());

        return(set_loop_test(set_sensor_values,test_sensor_values));

    }
#endif

    bool set_loop_test( std::vector<Sensor_set> set_sensor_values,
                        std::vector<Sensor_test> test_sensor_values) {

        //TODO: Add range checks of inputs against the underlying data structures

        int idx = 0;
        // Set sensors to passed set values
        for(auto &val: set_sensor_values) {

            std::cout << (int)val << " ";

            if(Sensor_set::inactive == val) {
                sensors_to_set_[idx]->set_state(false);
                std::cout << "(in),";
            }
            else if(Sensor_set::active == val) {
                sensors_to_set_[idx]->set_state(true);
                std::cout << "(ac),";
            }
            else {
                // Sensor_set::nochange
                // can't set sensor to indeterminate, ignore
                std::cout << "(ot),";
            }
            idx++;
        }
        std::cout << "\n";


        // Loop the logic
        logic_.loop();

        idx = 0;
        for(auto &val: test_sensor_values) {

            if(sensors_to_test_[idx]->is_indeterminate()) {
                std::cout << "(id) ";
            }
            else if(sensors_to_test_[idx]->is_active()) {
                std::cout << "(ac) ";
            }
            else {
                std::cout << "(in) ";
            }

            idx++;
        }

        std::cout << "\n";
        idx = 0;

        // Test sensors against passed test values
        for(auto &val: test_sensor_values) {

            if(Sensor_test::indeterminate == val) { // Indeterminate

                if(true != sensors_to_test_[idx]->is_indeterminate()) {
                    return false;
                }
            }
            else if(Sensor_test::inactive == val) {  // Inactive
               if(!(false == sensors_to_test_[idx]->is_indeterminate() &&
                    false == sensors_to_test_[idx]->is_active())) {
                   // Sensor needs to be not indeterminate and inactive
                   return false;
               }
            }

            else if(Sensor_test::active == val) {  // Active
                if(!(false == sensors_to_test_[idx]->is_indeterminate() &&
                     true == sensors_to_test_[idx]->is_active())) {
                    // Sensor needs to be not indeterminate and active
                    return false;
                }
            }
            else {
                // Invalid state to test
                return false;
            }

            idx++;
        }   // end for()

        // All tests passed, return success
        return true;
    }


private:
    Logic_interface &logic_;
    std::vector<Sensor_base*> sensors_to_set_;
    std::vector<Sensor_interface*> sensors_to_test_;


};

/*
 *     Simple_apb(std::initializer_list<Sensor_interface *> const & protected_sensors);

    void loop() override;

    Sensor_interface& down_tumbledown();
    Sensor_interface& up_tumbledown();

protected:
    std::vector<Sensor_interface*> protected_sensors_;
 */


} // namespace mr_signals

#endif /* TEST_MAST_TEST_HELPERS_H_ */
