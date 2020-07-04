/*
 * loop_funcs.cpp
 *
 *  Created on: Jun 26, 2020
 *      Author: ackpu
 */

#include "../loop_funcs.h"

Loop_interface::Loop_interface(Loop_collection &collection) {
    collection.attach(this);
}



