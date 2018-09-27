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



} // namespace mr_signals

#endif /* TEST_MAST_TEST_HELPERS_H_ */
