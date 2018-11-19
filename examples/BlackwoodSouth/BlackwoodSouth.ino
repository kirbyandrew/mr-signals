#include <StandardCplusplus.h>
#include <sensor_interface.h>
#include <single_switch_head.h>
#include <double_switch_head.h>
#include <quadln_s_head.h>
#include <logic_collection.h>
#include "base/switch_interface.h"
#include <ryg_logic.h>
#include <vector>
#include <mr_signals.h>   // Print()
#include <LocoNet.h>

#include "loconet/mrrwa_loconet_adapter.h"

using namespace mr_signals;

const size_t num_logic_interfaces = 5;

Logic_collection logic_collection(num_logic_interfaces);


const int tx_pin = 42;
const size_t num_sensors = 10;
const size_t tx_buffer_size = 6;

Mrrwa_loconet_adapter loconet_adapter(LocoNet, tx_pin, num_sensors, tx_buffer_size);


Active_sensor active_sensor;
Sensor_base s1,s2,s3;
Test_switch sw1,sw2;


Single_switch_head ss_head1("",sw1);
Single_switch_head ss_head2("",sw2);
Single_switch_head ss_head3("",sw2);

Double_switch_head ds_head1("",sw1,sw2);
Double_switch_head ds_head2("",sw1,sw2);
Double_switch_head ds_head3("",sw1,sw2);
Double_switch_head ds_head4("",sw1,sw2);


Quadln_s_head quad_head1("",sw1,sw2);
Quadln_s_head quad_head2("",sw1,sw2);
Quadln_s_head quad_head3("",sw1,sw2);
Quadln_s_head quad_head4("",sw1,sw2);
#if 0
Quadln_s_head2 quad_head11("",sw1,sw2);
Quadln_s_head2 quad_head12("",sw1,sw2);
Quadln_s_head2 quad_head13("",sw1,sw2);
Quadln_s_head2 quad_head14("",sw1,sw2);
#endif

Test_sensor lever_1;
Test_sensor lever_2;
Test_sensor push_key;
Test_head   head_1;
Test_head   head_2;
/*
Sensor_interlocked_head logical_head_1(head_1,push_key);
Interlocked_ryg_logic test_logic_1(logical_head_1,lever_1,{ &s1});

Sensor_interlocked_head logical_head_2(head_2,push_key);
Interlocked_ryg_logic test_logic_2(logical_head_2,lever_2,{ &s1});
*/


Lever_with_pushkey interlocked_lever_1(lever_1,push_key);
Interlocked_ryg_logic test_logic_1(logic_collection, head_1,interlocked_lever_1,{ &s1 });

Lever_with_pushkey interlocked_lever_2(lever_2,push_key);
//Interlocked_ryg_logic test_logic_2(head_2,interlocked_lever_2,{ &s1 });

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

template <typename T>
class Container {

public:
    void reserve(size_t size)
    {
        collection_.reserve(size);
    }

    size_t size()
    {
        return(collection_.size());
    }

    void attach(T *element)
    {
        collection_.push_back(element);
    }

protected:
    std::vector<T*> collection_;
};


class Logic_if {
public:
//  Container<Logic_interface> logics_;

  public:
    void reserve(size_t size)
    {
        collection_.reserve(size);
    }

    size_t size()
    {
        return(collection_.size());
    }

    void attach(Logic_interface *element)
    {
        collection_.push_back(element);
    }

protected:
    std::vector<Logic_interface*> collection_;

};

Logic_if logic;

// TODO: Use an initializer list to add a bunch of ints to check how efficient the re-allocation is
void setup() {
  // put your setup code here, to run once:
 // test_switch.loop();


//  Serial.begin(9600);
//  Serial.begin(57600);
  Serial.begin(115200);
  
  Serial << "mr_signals test" << "\r\n";

  Serial.println(F("Blackwood Tower"));
  Serial.write("free RAM : ");
  Serial.println(freeRam());

  std::vector<int> v;

  Serial.print(v.capacity(),HEX);
  Serial << v.capacity(),HEX;
  Serial << HEX << v.capacity();

  uint8_t val = 2;
  Serial << unsigned(val) << "\r\n";


  Serial.write("after declaring v : ");
  Serial.println(freeRam());

  Serial.write("v.capacity : ");
  Serial.println(v.capacity());

  Serial.write("v.size : ");
  Serial.println(v.size());


  v.push_back(1);

  Serial.write("after push_back(1) : ");
  Serial.println(freeRam());

  Serial.write("v.capacity : ");
  Serial.println(v.capacity());

  Serial.write("v.size : ");
  Serial.println(v.size());
  
  v.push_back(2);

  Serial.write("after push_back(2) : ");
  Serial.println(freeRam());

  Serial.write("v.capacity : ");
  Serial.println(v.capacity());

  Serial.write("v.size : ");
  Serial.println(v.size());

  
  v.push_back(3);

  Serial.write("after push_back(3) : ");
  Serial.println(freeRam());

  Serial.write("v.capacity : ");
  Serial.println(v.capacity());

  Serial.write("v.size : ");
  Serial.println(v.size());


  v.push_back(4);

  Serial.write("after push_back(4) : ");
  Serial.println(freeRam());

  Serial.write("v.capacity : ");
  Serial.println(v.capacity());

  Serial.write("v.size : ");
  Serial.println(v.size());

  v.reserve(10);

  Serial.write("after reserve : ");
  Serial.println(freeRam());

  Serial.write("v.capacity : ");
  Serial.println(v.capacity());

  Serial.write("v.size : ");
  Serial.println(v.size());

  v.push_back(5);

  Serial.write("after push_back(5) : ");
  Serial.println(freeRam());

  Serial.write("v.capacity : ");
  Serial.println(v.capacity());

  Serial.write("v.size : ");
  Serial.println(v.size());


  v.push_back(6);

  Serial.write("after push_back(6) : ");
  Serial.println(freeRam());

  Serial.write("v.capacity : ");
  Serial.println(v.capacity());

  Serial.write("v.size : ");
  Serial.println(v.size());

  
  Serial.println(__cplusplus);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
