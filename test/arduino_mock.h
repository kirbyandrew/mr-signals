/*
 * arduino_mock.h
 *
 *  Created on: Aug 2, 2018
 *      Author: ackpu
 */

#ifndef TEST_ARDUINO_MOCK_H_
#define TEST_ARDUINO_MOCK_H_
/**
 * Mock functions to replace the arduino library used by mr-signals-test
 */

/**
 * Mock for the Arduino millis function.  The original returns the time
 * elapsed in milliseconds elapsed since power on.
 *
 * This version returns a value that is set through the set_millis() function
 */
unsigned long millis(void);

/**
 * Sets the value returned by the millis() mock function
 *
 * Returns the new value of millis()
 *
 * @param val - new value of mills() to set
 */

unsigned long set_millis(const unsigned long val);


/**
 * API to initialize the millis count to 0 for each test
 */
void init_millis(void);


/******************** Digital I/O mocks *******************/

// From Arduino.h
#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2


void digitalWrite(uint8_t pin, uint8_t val);
uint8_t digitalRead(uint8_t pin);
void pinMode(uint8_t pin, uint8_t mode);
uint8_t getPinMode(uint8_t pin);



#endif /* TEST_ARDUINO_MOCK_H_ */
