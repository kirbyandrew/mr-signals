/*
 * logic_collection.h
 *
 *  Created on: Feb 19, 2018
 *      Author: ackpu
 */

#ifndef SRC_LOGIC_COLLECTION_H_
#define SRC_LOGIC_COLLECTION_H_

#include <vector>   // std::vector<>
#include <cstddef>  // size_t
#include "base/logic_interface.h"

namespace mr_signals {


/// Forward declaration for Logic_collection::attach_logic_interface()
class Logic_interface;


class Logic_collection {

public:
    /// Dimension the storage at construction so that the storage ::vector
    /// does not continue to inefficiently grow.
    /// A reference to the Logic_Collection is passed to each Logic_interface
    /// so that the interface can be attached to the collection.
    Logic_collection(size_t num_logic_interfaces)
    {
        logic_functions_.reserve(num_logic_interfaces);
    }

    /**
     * Attaches an instace of a Logic interface to the logic collection
     * Normally called by the Logic_interface collector
     * @param interface
     */
    void attach_logic_interface(Logic_interface *interface)
    {
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
    size_t logic_count()
    {
        return logic_functions_.size();
    }


    /// To be called in the main Arduino loop() so that the logic functions
    /// are periodically run
    void loop()
    {
        for(Logic_interface* logic : logic_functions_)  {
            logic->loop();
        }
    }

private:
    std::vector<Logic_interface *> logic_functions_;

};

};



#endif /* SRC_LOGIC_COLLECTION_H_ */
