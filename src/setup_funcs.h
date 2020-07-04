

#ifndef SETUP_FUNCS_H_
#define SETUP_FUNCS_H_


#include "./base/collection_base.h"

class Setup_collection;


class Setup_interface {
public:
  Setup_interface(Setup_collection&);
  virtual void setup() = 0;
};


class Setup_collection : public Collection_base<Setup_interface, &Setup_interface::setup>
{
public:
    Setup_collection(const collec_size size) : Collection_base(size) {}

};

#endif /* SETUP_FUNCS_H_ */
