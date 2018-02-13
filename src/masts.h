/*
 * masts.h
 *
 *  Created on: Feb 9, 2018
 *      Author: ackpu
 */

#ifndef SRC_MASTS_H_
#define SRC_MASTS_H_


#include "base/mast_interface.h"



namespace mr_signals {

class Standard_mast : public Mast_interface
{
public:
    Standard_mast();

    virtual void attach_head(Head_interface& head,
            Head_interface& protected_head,
            std::initializer_list<Sensor_interface *> const & protected_sensors)
                    override;


    void attach_head(Head_interface& head,
            std::initializer_list<Sensor_interface *> const & protected_sensors)
                    override;



    void loop() override;

protected:


    struct Mast_head {
        Head_interface& head;               // Reference as there must be a head
        Head_interface* protected_head;     // Pointer as there may not be a protected head. nullptr used when not present
        std::vector<Sensor_interface *> protected_sensors_;
    };

    bool process_head(Mast_head& head);

    std::vector<Mast_head> heads_;
};

/*
class Interlocked_mast : public Standard_interface
{
public:
    Interlocked_mast(

    );

    virtual void attach_head(Head_interface& head,
            Head_interface& protected_head,
            std::initializer_list<Sensor_interface *> const & protected_sensors)
                    override;


    void attach_head(Head_interface& head,
            std::initializer_list<Sensor_interface *> const & protected_sensors)
                    override;



    void loop() override;

private:

    Sensor_interface &lever_;
    Sensor_interface *automated_lever_;


    bool process_head(Mast_head& head);

    std::vector<Mast_head> heads_;
};

*/

}


#endif // SRC_MASTS_H_
