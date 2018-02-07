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
    virtual Head_aspect get_aspect() = 0;

    /**
     * Method to request a new aspect for the head
     *
     * @param aspect - the new aspect
     * @return bool -   true  = aspect was accepted / change was successful
     *                  false = aspect was rejected or change was not made
     *                  (switch could not be written etc)
     */
    virtual bool set_aspect(const Head_aspect) = 0;
    virtual bool loop() = 0;                        // TODO: Why does this return bool?
    virtual const char* get_name() = 0;

    virtual void set_held(bool)= 0;              // TODO : make an interim cass that holds the held
    virtual bool is_held() = 0;

    virtual ~Head_interface() = default;

    #define head_name_len 5
};

/**
 * Fixed_red_head
 *
 * An always red head for convenience
 */
class Fixed_red_head : public Head_interface
{
    Head_aspect get_aspect() override
    {
        return Head_aspect::red;
    }

    // Does nothing
    bool set_aspect(const Head_aspect aspect) override
    {
        if (aspect) {}
        return false;
    }
    bool loop() override
    {
        return false;
    }
    const char* get_name() override
    {
        return(nullptr);
    }

    void set_held(bool) override {}
    bool is_held() override
    {
        return false;
    }

};


const char* get_aspect_string(const Head_aspect);


}



#endif /* SRC_BASE_HEAD_INTERFACE_H_ */
