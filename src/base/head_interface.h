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

class Head_interface
{
public:

    /**
     * Method to request a new aspect for the head
     *
     * @param aspect - the new aspect
     * @return bool -   true  = aspect was accepted / change was successful
     *                  false = aspect was rejected or change was not made
     *                  (switch could not be written etc)
     */
    virtual bool request_aspect(const Head_aspect) = 0;
    virtual void loop() = 0;

    virtual Head_aspect get_aspect();   /// Get the current aspect of the head
    virtual const char* get_name();     /// Get the name of the head

    void set_held(const bool);
    bool is_held();

    Head_interface(const char* name);
    virtual ~Head_interface() = default;



protected:
    void set_aspect(Head_aspect aspect);    // Sets the internal aspect_ state


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

/// Get a string representation of the pass Head_aspect state
const char* get_aspect_string(const Head_aspect);


}



#endif /* SRC_BASE_HEAD_INTERFACE_H_ */
