#include <StandardCplusplus.h>
#include <mr_signals.h>
#include <logic_collection.h>
#include <triple_pin_head.h>
#include <pin_sensor.h>
#include <ryg_logic.h>

using namespace mr_signals;

const size_t num_logic_interfaces = 3;    
    
Logic_collection logic_collection(num_logic_interfaces);

Pin_sensor sensor_1(9);
Pin_sensor sensor_2(5);
Pin_sensor sensor_3(1);

Triple_pin_head head_1("Head1",10,11,12);
Triple_pin_head head_2("Head2",6,7,8);
Test_head       head_3("Head3");
// Or, if you want to construct head 3 on the breadboard, Triple_pin_head head_3("Head3",2,3,4);

Simple_ryg_logic logic_1(logic_collection, head_1, head_2, {&sensor_1});
Simple_ryg_logic logic_2(logic_collection, head_2, head_3, {&sensor_2});
Simple_ryg_logic logic_3(logic_collection, head_3,         {&sensor_3});



void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
