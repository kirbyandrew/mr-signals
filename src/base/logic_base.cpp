/*
 * logic.cpp
 *
 * Implementation for Logic_Collection and Logic_interface classes
 *
 *  Created on: Jun 13, 2020
 *      Author: ackpu
 */

#include "../logic_collection.h"
#include "logic_interface.h"


using namespace mr_signals;


    /// Dimension the storage at construction so that the storage ::vector
/// does not continue to inefficiently grow.
/// A reference to the Logic_Collection is passed to each Logic_interface
/// so that the interface can be attached to the collection.
Logic_collection::Logic_collection(size_t num_logic_interfaces) {
    init_size_ = num_logic_interfaces;

    logic_functions_.reserve(init_size_);
}

/**
 * Attaches an instace of a Logic interface to the logic collection
 * Normally called by the Logic_interface collector
 * @param interface
 */
void Logic_collection::attach_logic_interface(Logic_interface *interface) {
    logic_functions_.push_back(interface);
}

/**
 * Get the number of logic_functions that are attached to the
 * collection.  This is to be used to compare the final size
 * of the collection to the size that was dimensioned in the
 * constructor.
 *
 * @return
 */
size_t Logic_collection::logic_count() {
    return logic_functions_.size();
}

size_t Logic_collection::logic_init_size() {
    return init_size_;
}

/// To be called in the main Arduino loop() so that the logic functions
/// are periodically run
void Logic_collection::loop() {
    for (Logic_interface* logic : logic_functions_) {
        logic->loop();
    }
}


/**
 * As the only constructor, ensures that all subclasses will register with
 * a Logic_collection object so that their loop function is registered
 * with the collection (and is ideally then called at the required interval).
 *
 * @param collection
 */
Logic_interface::Logic_interface(Logic_collection &collection) {
    collection.attach_logic_interface(this);
}


