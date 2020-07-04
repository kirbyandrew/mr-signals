/*
 * loop_funcs.h
 *
 *  Created on: Jun 26, 2020
 *      Author: ackpu
 */

#ifndef LOOP_FUNCS_H_
#define LOOP_FUNCS_H_



#include "./base/collection_base.h"

class Loop_collection;


class Loop_interface {
public:
  Loop_interface(Loop_collection&);
  virtual void loop() = 0;
};


class Loop_collection : public Collection_base<Loop_interface,&Loop_interface::loop>
{
public:
    Loop_collection(const collec_size size) : Collection_base(size) {}

};




#endif /* LOOP_FUNCS_H_ */
