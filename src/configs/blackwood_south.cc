
#ifdef ARDUINO

#include "setup_funcs.h"
#include "loop_funcs.h"

#include "loconet/mrrwa_loconet_adapter.h"
#include "loconet/loconet_sensor.h"
#include "loconet/loconet_switch.h"


#include "helpers.h"
#include "logic_collection.h"

#include "ryg_logic.h"
#include "apb_logic.h"
#include "double_switch_head.h"
#include "single_switch_head.h"
#include "quadln_s_head.h"


#define DOUBLE_SWITCH_LOCONET_HEAD(name, sw1_addr, sw2_addr)\
Loconet_switch head_##name##_sw1(sw1_addr, &loconet);\
Loconet_switch head_##name##_sw2(sw2_addr, &loconet);\
Double_switch_head head_##name(#name, head_##name##_sw1, head_##name##_sw2);


#define SINGLE_SWITCH_LOCONET_HEAD(name, sw1_addr)\
Loconet_switch head_##name##_sw1(sw1_addr, &loconet);\
Single_switch_head head_##name(#name, head_##name##_sw1);


#define QUAD_LN_HEAD(name,sw1_addr,mid_addr)\
Loconet_switch head_##name##_sw1(sw1_addr, &loconet);\
Loconet_switch head_##name##_sw2(mid_addr, &loconet);\
Quadln_s_head head_##name(#name,head_##name##_sw1,head_##name##_sw2);

using namespace mr_signals;




Logic_collection logic_coll(35);



// Levers and Pushkeys 
Loconet_sensor		lever_1("L1", 24, loconet);
Loconet_sensor 		lever_2("L2", 23, loconet);
Loconet_sensor 		lever_3("L3", 22, loconet);
Loconet_sensor 		lever_5("L5", 21, loconet);
Loconet_sensor 		lever_6("L6", 1, loconet);
Inverted_sensor 	lever_6_inv(lever_6);
Loconet_sensor 		lever_8("L8", 2, loconet);
Inverted_sensor 	lever_8_inv(lever_8);
Loconet_sensor 		lever_10("L10", 3, loconet);
Loconet_sensor 		lever_11("L11", 4, loconet);
Loconet_sensor 		lever_auto("L12", 20, loconet);	// Lever 12
Loconet_sensor 		lever_13("L13", 5, loconet);
Inverted_sensor		lever_13_inv(lever_13);
Loconet_sensor 		lever_17("L17", 6, loconet);
Loconet_sensor 		lever_18("L18", 7, loconet);
Inverted_sensor 	lever_18_inv(lever_18);
Loconet_sensor 		lever_20("L20", 8, loconet);
Inverted_sensor 	lever_20_inv(lever_20);
Loconet_sensor 		lever_22("L22", 19, loconet);
Loconet_sensor 		lever_24("L24", 18, loconet);
Loconet_sensor 		pushkey_1("PK1", 17, loconet);
Lever_with_pushkey 	lever_1c(lever_1, pushkey_1);




// Track sensors 
Loconet_sensor t_1("T1",		52, loconet);
Loconet_sensor t_6("T6",        51, loconet);
Loconet_sensor t_8("T8",        50, loconet);
Loconet_sensor t_20("T20",      69, loconet);
Loconet_sensor t_24("T24",      49, loconet);
Loconet_sensor t_1107("T1107",	53, loconet);
Loconet_sensor t_1120("T1120", 	54, loconet);
Loconet_sensor t_1214("T1214", 	72, loconet);
Loconet_sensor t_1342("T1342", 	70, loconet);
Loconet_sensor t_ptAdel("tPtA", 71, loconet);
Loconet_sensor sw_feed_store("swFM", 65, loconet);
Loconet_sensor sw_bkwd_mill("swMi", 66, loconet);


// 48   Tracks 31-37
Loconet_sensor t_31("T31",		33, loconet); // T31 & T32
Loconet_sensor t_33("T33",		34, loconet);
Loconet_sensor t_34("T34",		35, loconet);
Loconet_sensor t_35("T35",		36, loconet);
Loconet_sensor t_36("T36",		37, loconet);
Loconet_sensor t_37("T37",		38, loconet);
Loconet_sensor sw_yelta("SwY",	39, loconet);
Loconet_sensor sw_marino("SwM",	40, loconet);

Simple_apb bkwd_yelta_apb(logic_coll,{&t_20,&t_1214,&t_1342});


Two_lever_interlock<Loconet_sensor, &lever_2, Loconet_sensor, &lever_5> levers_2_5(logic_coll);


class Signal_3_sensor: public Sensor_interface
{
public:
    bool is_active() override
    {
        if(lever_10.is_active())
        {
            // If Switch 10 is reversed (the lead), there is no 
			// protection; signal 3 can be cleared
            return false;
        }
        else
        {
            if( !lever_8.is_active()  ||    // Switch 8 must be reversed to enter the main from signal 3
                lever_6.is_active() ||      // Switch 6 must be normal (cannot enter Down Main from 3)
                t_8.is_active() ||       	// Track 8 must be clear
                t_1120.is_active())      	// Track 1120 must be clear
            {
                return true;                // Sensor 'true' (no go)
            }
            else
            {
                return false;               // Sensor inactive; signal can be cleared
            }
        }

        return false;
    }
    bool is_indeterminate() const override 
    {
      if( lever_10.is_indeterminate() ||	// Check all sensors used in is_active()
          lever_8.is_indeterminate()  ||
          lever_6.is_indeterminate()  ||
          t_8.is_indeterminate() ||
          t_1120.is_indeterminate()) 
	  {
        return true;     
      }
      return false;
    }
};

Signal_3_sensor signal_3_sensor;

// Heads (use macros to simplify)
DOUBLE_SWITCH_LOCONET_HEAD(1a, 	98,	 97)
DOUBLE_SWITCH_LOCONET_HEAD(1b,  100, 99)
Red_head_sensor head_1b_red(head_1b);
SINGLE_SWITCH_LOCONET_HEAD(1c,  45)

DOUBLE_SWITCH_LOCONET_HEAD(2c,	47, 48)
DOUBLE_SWITCH_LOCONET_HEAD(3c,	43, 44)
DOUBLE_SWITCH_LOCONET_HEAD(5a,	106,105)
DOUBLE_SWITCH_LOCONET_HEAD(5b,  41, 42)

DOUBLE_SWITCH_LOCONET_HEAD(22a, 114,113)
DOUBLE_SWITCH_LOCONET_HEAD(22b, 116,115)
DOUBLE_SWITCH_LOCONET_HEAD(22d, 17, 18)

DOUBLE_SWITCH_LOCONET_HEAD(24b, 118,117)
Red_head_sensor head_24b_red(head_24b);
SINGLE_SWITCH_LOCONET_HEAD(24c, 20)

DOUBLE_SWITCH_LOCONET_HEAD(1107a,110,109)
DOUBLE_SWITCH_LOCONET_HEAD(1107b,112,111)

DOUBLE_SWITCH_LOCONET_HEAD(1108a,108,107)

DOUBLE_SWITCH_LOCONET_HEAD(1213a,120,119)
DOUBLE_SWITCH_LOCONET_HEAD(1214a,124,123)
DOUBLE_SWITCH_LOCONET_HEAD(1214b,126,125)
QUAD_LN_HEAD(1342a,200,208)
QUAD_LN_HEAD(1343a,201,209)
QUAD_LN_HEAD(1381a,202,210)
QUAD_LN_HEAD(1382a,203,211)
QUAD_LN_HEAD(2412a,204,212)
QUAD_LN_HEAD(2413a,205,213)
QUAD_LN_HEAD(2430a,206,214)
QUAD_LN_HEAD(2431a,207,215)
DOUBLE_SWITCH_LOCONET_HEAD(2612a,21,22)



Fixed_red_head head_red;


//                                          head,    prot. head,    lever,   auto_lever,    Protected switches and track circuits 			

																							// TODO: Add Mill switch
Interlocked_ryg_logic 	l_1a(logic_coll,	head_1a,	head_22a,	lever_1, lever_auto, 	{&t_1,	&t_6, 			&lever_6, 		&lever_13, &sw_bkwd_mill	});
Interlocked_ryg_logic 	l_1b(logic_coll,	head_1b,	head_22b,	lever_1, 				{&t_6,	&t_8, &t_24, 	&lever_6_inv,	&lever_8,	&lever_13,	&lever_18,	&lever_20_inv});
Interlocked_ryg_logic 	l_1c(logic_coll,	head_1c,			 	lever_1c,  				{&t_6,	&t_8,			&lever_6_inv,	&lever_8_inv});

Interlocked_ryg_logic	l_2c(logic_coll,	head_2c,				lever_2,				{&t_8,					&lever_6,		&levers_2_5.lock_first});

Interlocked_ryg_logic	l_3c(logic_coll,	head_3c,				lever_3,				{&signal_3_sensor});

Interlocked_ryg_logic	l_5a(logic_coll,	head_5a,	head_1108a,	lever_5, lever_auto,	{&t_8,	&t_1120,		&lever_6,		&lever_8,	&lever_13,	&levers_2_5.lock_second});	// TODO: Implement sensor 3
Interlocked_ryg_logic	l_5b(logic_coll,	head_5b,				lever_5, 				{&t_8,	&t_1120,		&lever_6,		&lever_8,	&lever_13_inv,	&levers_2_5.lock_second});	// TODO: Implement sensor 3




//                                          head,    	prot. head, lever,   auto_lever,    Protected switches and track circuits 			

Interlocked_ryg_logic 	l_22a(logic_coll, 	head_22a, 	head_1213a,	lever_22,lever_auto,    {&t_20, &t_1214, &lever_20, &bkwd_yelta_apb.down_tumbledown()});
Interlocked_ryg_logic 	l_22b(logic_coll, 	head_22b,	head_1213a,	lever_22,				{&t_20, &t_1214, &lever_20_inv, &lever_18, &bkwd_yelta_apb.down_tumbledown()});	// TODO: Add interlock between 22B, 22D & 24
Interlocked_ryg_logic 	l_22d(logic_coll, 	head_22d,				lever_22,				{&t_20, &t_1214, &lever_20_inv, &lever_18_inv, &bkwd_yelta_apb.down_tumbledown()});

Interlocked_ryg_logic 	l_24b(logic_coll,	head_24b,	head_5a,	lever_24,lever_auto,	{&t_20,	&t_24,	 &lever_18, 	&lever_20_inv });
Interlocked_ryg_logic 	l_24c(logic_coll,	head_24c,				lever_24,				{&t_20,			 &lever_18_inv,	&lever_20_inv });


Simple_ryg_logic 		l_1107a(logic_coll,	head_1107a,	head_1a,							{&t_1107, &sw_feed_store}); 
Simple_ryg_logic 		l_1107b(logic_coll,	head_1107b,	head_1b,							{&t_1107, &sw_feed_store, &head_1b_red});

Simple_ryg_logic 		l_1108(logic_coll,	head_1108a,	head_red,							{});

Simple_ryg_logic 		l_1213(logic_coll, 	head_1213a,	head_1343a,							{&t_1342});

Simple_ryg_logic 		l_1214a(logic_coll,	head_1214a,	head_red,							{&t_1214, &bkwd_yelta_apb.up_tumbledown()});
Simple_ryg_logic 		l_1214b(logic_coll,	head_1214b,	head_24b,							{&t_1214, &head_24b_red,&bkwd_yelta_apb.up_tumbledown()});

// Yelta
Simple_ryg_logic 		l_1342(logic_coll, 	head_1342a,	head_1214a,							{&t_1342, &bkwd_yelta_apb.up_tumbledown()});
Simple_ryg_logic 		l_1343(logic_coll, 	head_1343a,	head_1381a,							{&t_31, &t_33, &sw_yelta});

Simple_ryg_logic 		l_1381(logic_coll, 	head_1381a,	head_2413a,							{&t_34, &t_35});
Simple_ryg_logic 		l_1382(logic_coll, 	head_1382a,	head_1342a,							{&t_31, &t_33, &sw_yelta});

// Marino
Simple_ryg_logic 		l_2412(logic_coll, 	head_2412a,	head_1382a,							{&t_34, &t_33});
Simple_ryg_logic 		l_2413(logic_coll, 	head_2413a,	head_2431a,							{&t_35, &t_36, &t_37, &sw_marino});

Simple_ryg_logic 		l_2430(logic_coll, 	head_2430a,	head_2412a,							{&t_35, &t_36, &t_37, &sw_marino});
Simple_ryg_logic 		l_2431(logic_coll, 	head_2431a,										{&t_ptAdel});

// Pt Adelaide 
Simple_ryg_logic 		l_2612(logic_coll, 	head_2612a,										{&t_ptAdel, &t_37});



#endif // ARDUINO
