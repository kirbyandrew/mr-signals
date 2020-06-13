/*
 * mast_interface.h
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#ifndef SRC_BASE_LOGIC_INTERFACE_H_
#define SRC_BASE_LOGIC_INTERFACE_H_


namespace mr_signals {


/**
 * Abstract interface for the logic that controls a head or heads
 *
 * Each concrete implementation is analogous to the relay control logic of a
 * traditional signal installation.  It integrates all other objects
 * that are involved in determining the aspect of a head, and realizes
 * the corresponding logic.
 *
 */

class Logic_interface {
public:

    virtual void loop() = 0;

    // TODO: Should this be a Logic interface factory, or force passing a Logic_collection
    // to the constructor?
    //
    // e.g. https://stackoverflow.com/questions/57472952/design-pattern-flexibility-issue-factory-method
    // https://stackoverflow.com/questions/26866867/circular-reference-of-two-class-objects-in-c
    // https://softwareengineering.stackexchange.com/questions/323130/if-a-has-b-and-b-holds-reference-of-a-is-it-a-flawed-design-need-to-be-fixed
    // https://softwareengineering.stackexchange.com/questions/306518/when-is-a-circular-reference-to-a-parent-pointer-acceptable
    //
    virtual ~Logic_interface () = default;

};


}   // namespace mr_signals


#endif /* SRC_BASE_LOGIC_INTERFACE_H_ */
