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
    virtual bool set_aspect(const Head_aspect) = 0;
    virtual bool loop() = 0;
    virtual char *get_name() = 0;       // TODO : return zstring?

    //    virtual void set_held(bool)= 0;              // TODO : make an interim cass that holds the held
    //    virtual bool is_held() = 0;

    virtual ~Head_interface() = default;
};

/**
 * Fixed_red_head
 *
 * An always red head for convenience
 */
class Fixed_red_head : public Head_interface
{
    Head_aspect get_aspect() override {
        return Head_aspect::red;
    }

    // Does nothing
    bool set_aspect(const Head_aspect aspect) override {
        if (aspect) {}
        return false;
    }
    bool loop() {
        return false;
    }
    char* get_name() {
        return(nullptr);
    }
};


const char *get_aspect_string(const Head_aspect);


}



#endif /* SRC_BASE_HEAD_INTERFACE_H_ */
