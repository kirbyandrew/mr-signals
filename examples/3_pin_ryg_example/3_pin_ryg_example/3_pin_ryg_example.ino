#include <StandardCplusplus.h>    // This must be included to allow access to C++11 features not in the Arduino library
#include <mr_signals.h>           // Include the mr_signals library into the build
#include <logic_collection.h>     // We will use Logic_collection below
#include <triple_pin_head.h>      // We will use Triple_pin_head below
#include <pin_sensor.h>           // We will use Pin_sensor below
#include <ryg_logic.h>            // We will use Simple_ryg_logic below

using namespace mr_signals;       // All mr_signals functions are within the mr_signals namespace; this line must follow the includes

// Declare the number of logic interfaces so that we can check this later
const size_t num_logic_interfaces = 3;    

// Create a logic collection to manage all the logics
Logic_collection logic_collection(num_logic_interfaces);

// Create the 3 sensors that are read directly from the listed pins
Pin_sensor sensor_1(9);
Pin_sensor sensor_2(5);
Pin_sensor sensor_3(1);


// Create the three heads
// head_1 and head_2 are 'concrete' heads (Triple_pin_head) and are created with the
// specific pins they will use for the Red, Yellow and Green LEDs
// head_3 is a 'logical' head that operates only in SW and has no physical outputs
// If you want to construct head_3 on the breadboard, a suggested set of pins is included
Triple_pin_head head_1("Head1",10,11,12);
Triple_pin_head head_2("Head2",6,7,8);
Test_head       head_3("Head3");
// Or, if you want to construct head 3 on the breadboard, Triple_pin_head head_3("Head3",2,3,4);



// Construct the logical connections between the heads and sensors...
Simple_ryg_logic logic_1(logic_collection, head_1, head_2, {&sensor_1});  // head_1 is protecting the track 
                                                                          // associated with sensor_1 and then head_2
Simple_ryg_logic logic_2(logic_collection, head_2, head_3, {&sensor_2});  // head_2 is protecting the track 
                                                                          // associated with sensor_2 and then head_3
Simple_ryg_logic logic_3(logic_collection, head_3,         {&sensor_3});  // head_3 is only protecting the track
                                                                          // associated with sensor_3

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup() {
  // Print a banner and the amount of free memory on the system
  Serial.begin(57600);
  Serial.println(F("3_pin_ryg_example"));
  Serial.println(F("Free Memory:"));
  Serial.println(freeRam());

  // Always check that we dimensioned logic_collection correctly
  if(logic_collection.logic_count() > num_logic_interfaces) {
    Serial.println(F("*** logic_count() > predefined size; inefficient reallocation has occured ***"));
    Serial.println(logic_collection.logic_count());
  }
}

void loop() {

  logic_collection.loop();

}
