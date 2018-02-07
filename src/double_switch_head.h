/*
 * double_switch_head.h
 *
 *  Created on: Feb 6, 2018
 *      Author: ackpu
 */

#ifndef SRC_DOUBLE_SWITCH_HEAD_H_
#define SRC_DOUBLE_SWITCH_HEAD_H_

#include "head_interface.h"
#include "sensor_interface.h"
#include "switch_interface.h"

namespace mr_signals {

// TODO: Can Head_interface hold some of this common implementation?  Name, get and set held?

// TODO:

class Double_switch_head : public Head_interface
{
    #define double_switch_name_len 5

public:

    Double_switch_head(const char *name, Switch_interface& switch_1, Switch_interface& switch_2);

    Head_aspect get_aspect() override;
    bool set_aspect(const Head_aspect) override;

    const char *get_name() override;

    // TODO: Why does this return bool?
    bool loop() override;

    void set_held(const bool) override;
    bool is_held() override;


private:
    Switch_interface& switch_1_;
    Switch_interface& switch_2_;
    char name_[head_name_len+1];
    Head_aspect aspect_;
    char held_ : 1;
};


} // namespace mr_signals

#endif /* SRC_DOUBLE_SWITCH_HEAD_H_ */
