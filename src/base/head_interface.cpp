/*
 * head_interface.cpp
 *
 *  Created on: Feb 2, 2018
 *      Author: ackpu
 */

#include <string.h>
#include "head_interface.h"

using namespace mr_signals;


Head_interface::Head_interface(const char* name)
{
    // Ensure safe string copy and terminated name_[]
    strncpy(name_, name, head_name_len);
    name_[head_name_len] = '\0';

    aspect_ = unknown;
    held_ = held_false;
}

void Head_interface::set_aspect(Head_aspect aspect)
{
    aspect_ = aspect;
}

Head_aspect Head_interface::get_aspect()
{
    return (Head_aspect) aspect_;
}

const char* Head_interface::get_name()
{
    return name_;
}

void Head_interface::set_held(const bool held)
{
    held_ = (aspect_ != unknown && true==held) ? held_true : held_false;
}

bool Head_interface::is_held()
{
    // Would ideally be (held_true == held_) but gcc appears to promote held_
    // to an int for this comparison, which yields a value of -1 rather than 1
    // so just compare to zero or non-zero
    return (held_) ? true : false;
}





/// Get a string representation of a head's aspect
/// Return a C-string as the F() macro cannot return a const char*,
/// and returning the char string allows for cross-target testing
const char* mr_signals::get_aspect_string(const Head_aspect aspect)
{
    switch(aspect) {
    case unknown:
        return "u";
    case dark:
        return "d";
    case red:
        return "r";
    case yellow:
        return "y";
    case green:
        return "g";
    default:
        break;      //fall through below to always return
    }

    return "e"; // error
}



