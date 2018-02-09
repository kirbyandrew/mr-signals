#include <sensor_interface.h>
#include <single_switch_head.h>
#include <double_switch_head.h>
#include "base/switch_interface.h"

using namespace mr_signals;

Active_sensor active_sensor;
Sensor_base s1,s2,s3;
Test_switch sw1,sw2;


Single_switch_head ss_head1("",sw1);
Single_switch_head ss_head2("",sw2);
Single_switch_head ss_head3("",sw2);

/*
Double_switch_head ds_head1("",sw1,sw2);
Double_switch_head ds_head2("",sw1,sw2);
Double_switch_head ds_head3("",sw1,sw2);
*/


void setup() {
  // put your setup code here, to run once:
 // test_switch.loop();

}

void loop() {
  // put your main code here, to run repeatedly:

}
