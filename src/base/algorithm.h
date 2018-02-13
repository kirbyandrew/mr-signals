/*
 * algorithm.h
 *
 * Patch header file to implement all_of, any_of and none_of which are not
 * implemented in the default StandardCplusplus library
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#ifndef SRC_BASE_ALGORITHM_H_
#define SRC_BASE_ALGORITHM_H_

#include <algorithm>

namespace std {


// Check if using the uclib library from StandardCplusplus
// If we are using the uclib, define the missing template functions
#ifdef __UCLIBCXX_MAJOR__


template<class InputIt, class UnaryPredicate>
InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate q)
{
    for (; first != last; ++first) {
        if (!q(*first)) {
            return first;
        }
    }
    return last;
}


template<class InputIt, class UnaryPredicate>
bool all_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return std::find_if_not(first, last, p) == last;
}


template<class InputIt, class UnaryPredicate>
bool any_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return std::find_if(first, last, p) != last;
}


template<class InputIt, class UnaryPredicate>
bool none_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return std::find_if(first, last, p) == last;
}

#endif

}



#endif /* SRC_BASE_ALGORITHM_H_ */
