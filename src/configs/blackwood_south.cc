
#ifdef ARDUINO


#include "loconet/mrrwa_loconet_adapter.h"
#include "loconet/loconet_sensor.h"
#include "loconet/loconet_switch.h"

#include "helpers.h"
#include "logic_collection.h"

#include "ryg_logic.h"
#include "double_switch_head.h"
#include "single_switch_head.h"

using namespace mr_signals;

/*
const int tx_pin = 47;
const size_t num_sensors = 10;
const size_t tx_buffer_size = 6;

Mrrwa_loconet_adapter loconet(LocoNet, tx_pin, num_sensors, tx_buffer_size);
*/

Logic_collection logic_collection(6);



// Levers and Pushkeys 
Loconet_sensor lever_1("L1", 24, loconet);
Loconet_sensor lever_6("L6", 1, loconet);

Inverted_sensor lever_6_inv(lever_6);
Loconet_sensor lever_8("L8", 3, loconet);


Loconet_sensor  lever_auto("L12", 20, loconet);	// Lever 12
Loconet_sensor pushkey_1("PK1", 17, loconet);

Lever_with_pushkey lever_1c(lever_1, pushkey_1);



// Track sensors 
Loconet_sensor t_1("T1",52,loconet);
Loconet_sensor t_1107("T1107",53,loconet);


// Output switches for signals 
Loconet_switch mast1_a_sw1(98, &loconet);
Loconet_switch mast1_a_sw2(97, &loconet);
Loconet_switch mast1_b_sw1(100, &loconet);
Loconet_switch mast1_b_sw2(99, &loconet);
Loconet_switch mast1_c_sw(45, &loconet);


Loconet_switch mast1107_a_sw1(110, &loconet);
Loconet_switch mast1107_a_sw2(109, &loconet);
Loconet_switch mast1107_b_sw1(112, &loconet);
Loconet_switch mast1107_b_sw2(111, &loconet);

// Signal Heads 
Double_switch_head head_1a("1A", mast1_a_sw1, mast1_a_sw2);
Double_switch_head head_1b("1B", mast1_b_sw1, mast1_b_sw2);
Single_switch_head head_1c("C", mast1_c_sw);

Red_head_sensor head_1b_red(head_1b);

Double_switch_head head_1107a("1107A", mast1107_a_sw1, mast1107_a_sw2);
Double_switch_head head_1107b("1107B", mast1107_b_sw1, mast1107_b_sw2);

Fixed_red_head head_red;



Simple_ryg_logic l_1107a(logic_collection,head_1107a,head_1a,{&t_1107});
Simple_ryg_logic l_1107b(logic_collection,head_1107b,head_1b,{&t_1107, &head_1b_red});

//                                          head,    prot. head,    lever,   auto_lever,    Protected switches and track circuits 			
                                                        // h_22a,
Interlocked_ryg_logic l_1a(logic_collection,head_1a,	head_red,	lever_1, lever_auto, 	{&t_1});//,&lever_6);
Interlocked_ryg_logic l_1b(logic_collection,head_1b,	head_red,	lever_1, 				{&t_1,&lever_6_inv});//,&switch_8});
Interlocked_ryg_logic l_1c(logic_collection,head_1c,	lever_1c,  							{&t_1,&lever_6});


#endif // ARDUINO
