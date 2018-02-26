/*
 * logic_collection.h
 *
 *  Created on: Feb 19, 2018
 *      Author: ackpu
 */

#ifndef SRC_LOGIC_COLLECTION_H_
#define SRC_LOGIC_COLLECTION_H_

#include <vector>
#include <initializer_list>
#include "base/logic_interface.h"

namespace mr_signals {

class Logic_collection {

public:
    /// All logic objects must be passed at construction;
    /// Ensures fully initialized object, and eliminates memory loss through
    /// continually growing the storage ::vector if they were passed in one at
    /// a time
    Logic_collection(std::initializer_list<Logic_interface *> const & logic_functions) :
        logic_functions_(logic_functions)
    {
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
