/*
 * head_interface.h
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */

#ifndef SRC_BASE_HEAD_INTERFACE_H_
#define SRC_BASE_HEAD_INTERFACE_H_

namespace mr_signals {


enum Head_aspect
{
    unknown,
    dark,
    red,
    yellow,
    green,
    max_head_aspect /// For range checking: valid: >= unknown && < max_head_aspect
};



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
    virtual Head_aspect get_aspect();

    /// Get the name of the head
    virtual const char* get_name();

    /// 'Lock' the current aspect of the head. Ignored if the state is unknown
    void set_held(const bool);

    /// Indicate whether the head's aspect is currently locked
    bool is_held();


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
    virtual bool request_outputs(Head_aspect aspect) = 0;

private:
    #define head_name_len 5
    char name_[head_name_len+1];    /// Name of the head.  Char array more RAM efficient than std::string

    char aspect_ : 4;               /// Lowest size internal representation of the Head_aspect enum

    #define held_true 1
    #define held_false 0
    char held_ : 1;                 /// Aspect of the head is being held (locked)

};

/**
 * Fixed_red_head
 *
 * An always red head for convenience
 */
class Fixed_red_head : public Head_interface
{

    Fixed_red_head() : Head_interface("") {}

    Head_aspect get_aspect() override
    {
        return Head_aspect::red;
    }

    // Does nothing
    bool request_aspect(const Head_aspect aspect) override
    {
        if (aspect) {}
        return false;
    }
    void loop() override
    {
    }

};

/**
 * Test class for the unit tests
 */
class Test_head : public Head_interface
{
public:
    Test_head() : Head_interface(""), aspect_(unknown),lock_(false) {}

    bool request_aspect(const Head_aspect aspect) override
    {
        if(lock_) {
            return false;
        }
        else {
            aspect_ = aspect;
            return true;
        }
    }

    void loop() override {}

    Head_aspect get_aspect() override
    {
        return aspect_;
    }

    bool request_outputs(Head_aspect) override { return false;}


private:
    Head_aspect aspect_;
    bool lock_;
};

/// Get a string representation of the pass Head_aspect state
const char* get_aspect_string(const Head_aspect);


}



#endif /* SRC_BASE_HEAD_INTERFACE_H_ */
