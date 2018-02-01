/*
 * sensor.h
 *
 *  Created on: Jan 31, 2018
 *      Author: ackpu
 */

#ifndef SENSOR_H_
#define SENSOR_H_

namespace mr_signals {

/**
 * Fundamental sensor interface for classes that use sensor states
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
     * @return State of the sensor true/false =  active/inactive
     */
    virtual bool get_state() = 0;

    /**
     * Determine whether the state of the sensor is known
     * At starup, the state of a sensor may now be known until the state
     * is received from a bus or polled from an input
     * @return true = state not know, false = state is known, and .get_state() can be used
     */
    virtual bool is_indeterminate() = 0;

    virtual ~Sensor_interface() {}
};



/**
 * The sensor base class for any non-trivial sensor implementation to inherit from.
 * Trivial implementations (e.g. those with an invariant state) can inherit directly
 * from Sensor_interface
 *
 * In addition to the functions provided by Sensor_interface, supports a method to
 * set the state of the sensor
 */
class Sensor_base : public Sensor_interface {

public:

    Sensor_base() { indeterminate_ = true; }

    /**
     * Obtain the state of the sensor
     * Only use if .is_indeterminate() == false
     * @return State of the sensor true/false =  active/inactive
     */
    bool get_state() override;

    /**
     * Determine whether the state of the sensor is known
     * At starup, the state of a sensor may now be known until the state
     * is received from a bus or polled from an input
     * @return
     */
    bool is_indeterminate() override;

    /// Set the state of the sensor true or false
    /** \brief Allows the state of the sensor to be set (active/inactive = true/false)
     * \param state - true/false = active/inactive
     * \return true if the sensor's state changed
     */
    bool set_state(bool state);

protected:
    bool indeterminate_;    /// Indicates that the sensor state is not known when true (set_state() hasn't been called)
    bool state_;            /// State of the sensor (active/inactive = true/false)

};


/** Active_sensor
 *  Utility class for when an active sensor is needed to be passed to
 *  something that expects a Sensor_interface
 *  Will always indicate that the sensor value is known and is active
 */

class Active_sensor : public Sensor_interface {
public:
    bool get_state() override { return true; }
    bool is_indeterminate() override { return false; }
};



}   // namespace mr_signals



#endif /* SENSOR_H_ */
