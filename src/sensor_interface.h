/*
 * sensor.h
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <stdint.h>

namespace mr_signals {

/**
 * Sensor interface for other classes that use sensors
 *
 * Sensors are used to model track circuits, inter-locking lever handles,
 * push buttons etc.; essentially anything that has a active, inactive or
 * unknown value.
 * Later classes specialize this interface to connect it to the physical
 * input to the system (bus, discrete input, etc); the method to set the
 * state is deferred to each later class
 *
 * This interface only allows a user to determine whether the state of the
 * sensor is known, and if it is, its current state
 *
 * This class does not provide an interface to set the sensor state to protect
 * the sensors from manipulation by consuming objects
 */
class Sensor_interface {
public:
    /**
     * Obtain the state of the sensor
     * Only use if .is_indeterminate() == false
     *
     * The function is not const as derived classes use this function to
     * change internal states when called
     *
     * @return State of the sensor true/false =  active/inactive
     */
    virtual bool is_active()  = 0;

    /**
     * Determine whether the state of the sensor is known
     * At starup, the state of a sensor may now be known until the state
     * is received from a bus or polled from an input
     * @return true = state not know, false = state is known, and .get_state() can be used
     */
    virtual bool is_indeterminate() const = 0;

    virtual ~Sensor_interface() = default;
};



/**
 * The sensor base class for any non-trivial sensor implementation to inherit from.
 *
 * Trivial implementations (e.g. those with an invariant state) can inherit directly
 * from Sensor_interface
 *
 * In addition to the functions provided by Sensor_interface, supports a method to
 * set the state of the sensor
 *
 * Global variable memory
 * First object   : 15 bytes
 * Subsequent     : +3 bytes
 */
class Sensor_base : public Sensor_interface {

public:

    Sensor_base() { indeterminate_ = sensor_active; }

    /**
     * Obtain the state of the sensor
     * Only use if .is_indeterminate() == false
     * @return State of the sensor true/false =  active/inactive
     */
    bool is_active() override;

    /**
     * Determine whether the state of the sensor is known
     * At starup, the state of a sensor may now be known until the state
     * is received from a bus or polled from an input
     * @return
     */
    bool is_indeterminate() const override;

    /// Set the state of the sensor true or false
    /** \brief Allows the state of the sensor to be set (active/inactive = true/false)
     * \param state - true/false = active/inactive
     * \return true if the sensor's state changed
     */
    bool set_state(const bool state);

protected:
    enum
    {
        sensor_inactive=0,
        sensor_active
    };


    uint8_t state_ : 1;            /// The current state of the sensor (0 = inactive, 1 = active)
    uint8_t indeterminate_: 1;     /// Indicates that the state of the sensor is
                                /// not yet known (.set_state() has not been called) when 1
};


/** Active_sensor
 *  Utility class for when an active sensor is needed to be passed to
 *  something that expects a Sensor_interface
 *  Will always indicate that the sensor value is known and is active
 */

class Active_sensor : public Sensor_interface {
public:
    bool is_active() override { return true; }
    bool is_indeterminate() const override { return false; }
};


/**
 * Wrapper to allow the inverted state of a sensor to be passed to to an
 * object that uses Sensor_interface.
 */
class Inverted_sensor : public Sensor_interface {
public:
    Inverted_sensor(Sensor_interface& sensor);

    bool is_active() override;

    bool is_indeterminate() const override;


private:
    Sensor_interface& sensor_;
};

class Test_sensor : public Sensor_base {
public:
    Test_sensor() {}
    Test_sensor(bool state) { set_state(state); }
};



}   // namespace mr_signals


#endif /* SENSOR_H_ */
