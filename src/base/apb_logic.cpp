/*
 * apb_sensor.cpp
 *
 *  Created on: Nov 25, 2018
 *      Author: ackpu
 */

#include <cstddef>      // size_t
#include <apb_logic.h>
#include "algorithm.h"

using namespace mr_signals;



Simple_apb::Simple_apb(std::initializer_list<Sensor_interface *> const & protected_sensors) :
        protected_sensors_(protected_sensors)
{
    // Leave the tumbdown_sensors in their default (indeterminate) state
}

void Simple_apb::loop()
{
    // First check that none of the sensors are indeterminate
    if (std::none_of(protected_sensors_.begin(),
                    protected_sensors_.end(),
                    [](Sensor_interface* sensor) {return sensor->is_indeterminate();})) {

        // Check for a completely empty block
        if (std::none_of(protected_sensors_.begin(),
                        protected_sensors_.end(),
                        [](Sensor_interface* sensor) {return sensor->is_active();})) {

            // No protected sensors are active, clear the tumble downs
            up_tumbledown_sensor.set_state(false);
            down_tumbledown_sensor.set_state(false);
        }
        else {  // At least one sensor is active, none are indeterminate

            if(protected_sensors_.front()->is_active() &&   // First block is occupied
                !down_tumbledown_sensor.is_active()) {      // Is not a train heading in the up direction
                                                            // (e.g. leaving rather than entering the protected blocks)

                // It appears that a train is entering the first block in the
                // down direction, so set the up direction tumbledown
                up_tumbledown_sensor.set_state(true);
            }

            if(protected_sensors_.back()->is_active() &&    // Last block is occupied
                !up_tumbledown_sensor.is_active()) {        // Is not a train heading in the down direction
                                                            // (e.g. leaving rather than entering the protected blocks)

                down_tumbledown_sensor.set_state(true);
            }

        }

    }   // Sensor indeterminate check
}


Sensor_interface& Simple_apb::down_tumbledown()
{
    return dynamic_cast<Sensor_interface&>(down_tumbledown_sensor);
}

Sensor_interface& Simple_apb::up_tumbledown()
{
    return dynamic_cast<Sensor_interface&>(up_tumbledown_sensor);
}






Full_apb::Full_apb(std::initializer_list<Sensor_interface *> const & protected_sensors) :
    protected_sensors_(protected_sensors) {

    // Check protection against an empty initializer list being passed
    // The tumbledown sensor get functions have to return something, so
    // at least one sensor in each is always allocated
    size_t num_sensors = (protected_sensors_.size() > 0) ? protected_sensors_.size() : 1;


    down_tumbledown_sensors_.reserve(num_sensors);
    up_tumbledown_sensors_.reserve(num_sensors);

}

void Full_apb::loop() {

}


Sensor_interface& Full_apb::down_tumbledown_num(uint8_t num) {
    size_t idx = num < down_tumbledown_sensors_.size() ? down_tumbledown_sensors_.size() : 0;

    // Can't cast a pointer to a reference directly; so use intermediate local pointer
    Sensor_interface *sensor = static_cast<Sensor_interface*>(down_tumbledown_sensors_[idx]);
    return *sensor;
}

Sensor_interface& Full_apb::up_tumbledown_num(uint8_t num) {
    size_t idx = num < up_tumbledown_sensors_.size() ? up_tumbledown_sensors_.size() : 0;

    // Can't cast a pointer to a reference directly; so use intermediate local pointer
    Sensor_interface *sensor = static_cast<Sensor_interface*>(up_tumbledown_sensors_[idx]);
    return *sensor;
}

/*
 * // vector::at
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector (10);   // 10 zero-initialized ints

  // assign some values:
  for (unsigned i=0; i<myvector.size(); i++)
    myvector.at(i)=i;

  std::cout << "myvector contains:";
  for (unsigned i=0; i<myvector.size(); i++)
    std::cout << ' ' << myvector.at(i);
  std::cout << '\n';

  return 0;
}
 */

/*
 * tatic_cast
static_cast is used for cases where you basically want to reverse an implicit conversion, with a few restrictions and additions. static_cast performs no runtime checks. This should be used if you know that you refer to an object of a specific type, and thus a check would be unnecessary. Example:

void func(void *data) {
  // Conversion from MyClass* -> void* is implicit
  MyClass *c = static_cast<MyClass*>(data);
  ...
}

int main() {
  MyClass c;
  start_thread(&func, &c)  // func(&c) will be called
      .join();
}
In this example, you know that you passed a MyClass object, and thus there isn't any need for a runtime check to ensure this.

dynamic_cast
dynamic_cast is useful when you don't know what the dynamic type of the object is. It returns a null pointer if the object referred to doesn't contain the type casted to as a base class (when you cast to a reference, a bad_cast exception is thrown in that case).

if (JumpStm *j = dynamic_cast<JumpStm*>(&stm)) {
  ...
} else if (ExprStm *e = dynamic_cast<ExprStm*>(&stm)) {
  ...
}
You cannot use dynamic_cast if you downcast (cast to a derived class) and the argument type is not polymorphic. For example, the following code is not valid, because Base doesn't contain any virtual function:

struct Base { };
struct Derived : Base { };
int main() {
  Derived d; Base *b = &d;
  dynamic_cast<Derived*>(b); // Invalid
}
An "up-cast" (cast to the base class) is always valid with both static_cast and dynamic_cast, and also without any cast, as an "up-cast" is an implicit conversion.

Regular Cast
These casts are also called C-style cast. A C-style cast is basically identical to trying out a range of sequences of C++ casts, and taking the first C++ cast that works, without ever considering dynamic_cast. Needless to say, this is much more powerful as it combines all of const_cast, static_cast and reinterpret_cast, but it's also unsafe, because it does not use dynamic_cast.

In addition, C-style casts not only allow you to do this, but they also allow you to safely cast to a private base-class, while the "equivalent" static_cast sequence would give you a compile-time error for that.

Some people prefer C-style casts because of their brevity. I use them for numeric casts only, and use the appropriate C++ casts when user defined types are involved, as they provide stricter checking.

shareimprove this answer
edited Jul 9 '17 at 3:23

proski
 *
 */


/*
protected:
    Full_apb() {} // Cannot instantiate without parameters
    std::vector<Sensor_interface*> protected_sensors_;

    // TODO: Should this be a vector or something else?  Could even just be a simple array, are they iterated over?
    std::vector<Sensor_base*> down_tumbledown_sensors;
    std::vector<Sensor_base*> up_tumbledown_sensors;
};

*/
