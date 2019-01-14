/*
 * arduino_mock.cpp
 *
 *  Created on: Aug 2, 2018
 *      Author: ackpu
 *
 *  Mocks for used Arduino library functions and test enabled functions
 *
 */

#include <cstdint>
#include "arduino_mock.h"

unsigned long millis_val = 0;


unsigned long millis(void)
{
    return(millis_val);
}

unsigned long set_millis(const unsigned long val)
{
    millis_val = val;

    return(millis());
}

void init_millis(void)
{
    set_millis(0L);
}



const uint8_t num_digital_io = 14;

uint8_t dio_mode[num_digital_io] = {INPUT,INPUT,INPUT,INPUT,INPUT,INPUT,INPUT,INPUT,INPUT,INPUT,INPUT,INPUT,INPUT,INPUT};
uint8_t dio_val[num_digital_io] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};

void pinMode(uint8_t pin, uint8_t mode) {
    if(pin < num_digital_io) {
        dio_mode[pin] = mode;
    }
}

uint8_t getPinMode(uint8_t pin) {
    return(dio_mode[pin]);
}

void digitalWrite(uint8_t pin, uint8_t val) {
    if(pin < num_digital_io) {
        dio_val[pin] = val;
    }
}

uint8_t digitalRead(uint8_t pin) {
    if(pin < num_digital_io) {
        if(HIGH == dio_val[pin]) {
            return HIGH;
        }
    }
    return LOW;
}



