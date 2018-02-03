/*
 * head_interface.cpp
 *
 *  Created on: Feb 2, 2018
 *      Author: ackpu
 */


#include "head_interface.h"

using namespace mr_signals;



const char *mr_signals::get_aspect_string(const Head_aspect aspect)
{
    switch(aspect) {
    case unknown:   return "u";
    case dark:      return "d";
    case red:       return "r";
    case yellow:    return "y";
    case green:     return "g";
    }

    return "e"; // error
}



