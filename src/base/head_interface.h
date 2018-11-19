/*
 * head_interface.h
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */

#ifndef SRC_BASE_HEAD_INTERFACE_H_
#define SRC_BASE_HEAD_INTERFACE_H_

#include <stdint.h>

namespace mr_signals {

// Supported aspects for a single signal head
// Constrain to single byte to save space
// Using uint8_t would be better, but there  arduino doesn't appear to like #include <cstdint>
enum class Head_aspect : uint8_t
{
    unknown,
    dark,
    red,
    yellow,
    green,
    max_head_aspect /// For range checking: valid: >= unknown && < max_head_aspect
};

inline Head_aspect operator++(Head_aspect& aspect,int)
{
    return aspect = static_cast<Head_aspect>(static_cast<uint8_t>(aspect)+1);
}



/**
 * Base interface for signal heads
 *
 * This class should be inherited from to realize a specific head's
 * implementation, whether that be to use switches, discrete outputs,
 * whatever
 */

class Head_interface
{
public:

    /**
     * Method to request a new aspect for the head
     *
     * A default implementation of common logic is provided by this class,
     * with specialization being implemented through overriding
     * request_outputs(), but the entire function can be overidden if needed
     *
     * @param aspect - the new aspect
     * @return bool -   true  = aspect was accepted / change was successful
     *                  false = aspect was rejected or change was not made
     *                  (switch could not be written etc)
     */
    virtual bool request_aspect(const Head_aspect);


    /// Allows anything attached to the head to have its processing loop executed
    virtual void loop() = 0;

    /// Get the current aspect of the head
    virtual Head_aspect get_aspect() const;

    /// Get the name of the head
    virtual const char* get_name() const;

    /// 'Lock' the current aspect of the head. Ignored if the state is unknown
    virtual void set_held(const bool);

    /// Indicate whether the head's aspect is currently locked
    virtual bool is_held() const;


    Head_interface(const char* name);
    virtual ~Head_interface() = default;



protected:
    /// Sets the internal aspect_ state
    void set_aspect(Head_aspect aspect);

    /**
     * Handles the concrete setting of output states when called
     * by the default implementation of ::request_aspect()
     *
     * Each concrete head class must implement this to control the
     * specific output management to realize each aspect it supports
     *
     * @param aspect -  the requested aspect
     * @return bool -   true  = Outputs were successfully set (or queued)
     *                  false = The output could not be set or and invalid
     *                          aspect was requested
     */
    virtual bool request_outputs(Head_aspect aspect);

private:
    static const int head_name_len = 5;
    char name_[head_name_len+1];        /// Name of the head.  Char array more RAM efficient than std::string

    uint8_t aspect_ : 4;               /// Lowest size internal representation of the Head_aspect enum

    enum
    {
        held_false=0,
        held_true
    };
    uint8_t held_ : 1;                 /// Aspect of the head is being held (locked)

};

/**
 * Fixed_red_head
 *
 * An always red head for convenience
 */
class Fixed_red_head : public Head_interface
{
public:
    Fixed_red_head() : Head_interface("") {}

    Head_aspect get_aspect() const override
    {
        return Head_aspect::red;
    }

    // Does nothing
    bool request_aspect(const Head_aspect aspect) override
    {
        if (Head_aspect::unknown == aspect) {}
        return false;
    }
    void loop() override
    {
    }

protected:
    bool request_outputs(Head_aspect) override
    {
        return false;
    }
};

/**
 * Test class for the unit tests
 */
class Test_head : public Head_interface
{
public:
    Test_head(const char* name = "") : Head_interface(name) {}

    bool request_aspect(const Head_aspect aspect) override
    {
        if(is_held()) {
            return false;
        }
        else {
            set_aspect(aspect);
            return true;
        }
    }

    void loop() override {}


    bool request_outputs(Head_aspect) override { return false;}


};


/// Get a string representation of the pass Head_aspect state
const char* get_aspect_string(const Head_aspect);


}



#endif /* SRC_BASE_HEAD_INTERFACE_H_ */
