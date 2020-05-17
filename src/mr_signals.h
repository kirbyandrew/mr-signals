/*
 * mr-signals.h
 *
 *  Created on: Aug 8, 2018
 *      Author: ackpu
 */

#ifndef SRC_MR_SIGNALS_H_
#define SRC_MR_SIGNALS_H_


/*
 * Provide stream access to Arduino Serial class.
 * Instead of
 * Serial.print("Var is : ");
 * Serial.println(var);
 *
 * use
 * Serial << "Var is : " << var << "\r\n";
 */
#ifdef ARDUINO

#include <Arduino.h>

// TODO : Look up streaming class

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

enum _EndLineCode { endl };
inline Print &operator <<(Print &obj, _EndLineCode arg) { obj.println(); return obj; }


#else

// For non-Arduino builds, use std::cout and associated operators for Serial <<
#include <iostream>

#define Serial std::cout
#define HEX std::uppercase << std::hex      // Arduino prints upper case hex
#define endl std::endl

#define F(x) x                              // Remove Arduino F macro

#endif


#endif // SRC_MR_SIGNALS_H_
