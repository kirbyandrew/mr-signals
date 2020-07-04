/*
 * setup_funcs.cpp
 *
 *  Created on: Jun 26, 2020
 *      Author: ackpu
 */

#include "../setup_funcs.h"

Setup_interface::Setup_interface(Setup_collection &collection) {
    collection.attach(this);
}



