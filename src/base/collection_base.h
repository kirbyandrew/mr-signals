/*
 * collection_base.h
 *
 *  Created on: Jun 26, 2020
 *      Author: ackpu
 */

#ifndef SRC_BASE_COLLECTION_BASE_H_
#define SRC_BASE_COLLECTION_BASE_H_

#include <vector>
#include <stdint.h>

typedef uint8_t collec_size;

template <class T,void (T::*method)()>
class Collection_base {

public:

    Collection_base(const collec_size size) : init_size_(size ){
        collected_objects_.reserve(init_size_);
    }

    void attach(T* obj) {
        collected_objects_.push_back(obj);
    }

    void execute() {
        for(T* obj : collected_objects_) {
            (obj->*method)();
        }
    }

    collec_size count() const {
        return collected_objects_.size();
    }

    collec_size init_size() const {
        return init_size_;
    }


protected:
    std::vector<T*> collected_objects_;
    collec_size init_size_;
};



#endif /* SRC_BASE_COLLECTION_BASE_H_ */
