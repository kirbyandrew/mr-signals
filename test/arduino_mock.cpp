/*
 * arduino_mock.cpp
 *
 *  Created on: Aug 2, 2018
 *      Author: ackpu
 *
 *  Mocks for used Arduino library functions and test enabled functions
 *
 */

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

