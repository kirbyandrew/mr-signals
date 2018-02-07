/*
 * head_interface.cpp
 *
 *  Created on: Feb 2, 2018
 *      Author: ackpu
 */


#include "head_interface.h"

using namespace mr_signals;


/// Get a string representation of a head's aspect
/// Return single chars as the F() macro cannot return a const char*,
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



