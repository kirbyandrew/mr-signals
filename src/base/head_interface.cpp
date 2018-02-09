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


/**
 * Default wrapper logic for requesting the aspect of a given head
 *
 * Setting out outputs is handled in the derived class' implementation of
 * set_aspect().  This class handles:
 *
 * - Skipping if the head is held
 * - Checking that the requested aspect is different from the current aspect
 *   before attempting to change the outputs
 * - Only changing the head's current aspect if the outputs were successfully
 *   set
 * - Reporting success if the current aspect is requested, whether held or not
 *
 *
 * @param aspect: The head's aspect being requested
 * @return true: The head is set (or setting to) the requested aspect
 *         false: The head could not change to the requested aspect
 */
bool Head_interface::request_aspect(const Head_aspect aspect)
{
    bool result = false;    // Assume unsuccessful

    if(get_aspect() == aspect) {
        // If the same aspect is requested as is currently set, irrespective
        // of the hold state, return success
        result = true;
    }
    else {
        if (!is_held()) {
            if (aspect != get_aspect()) {

                // If the head's aspect isn't being held, and a different
                // aspect is being requested, attempt to set the outputs
                if (request_outputs(aspect)) {

                    // If the output set to the requested aspect, update
                    // the current aspect and return success
                    set_aspect(aspect);
                    result = true;
                }
            }
        }
    }

    return result;
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



