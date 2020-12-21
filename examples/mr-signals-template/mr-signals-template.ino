#include <StandardCplusplus.h>

#include <mr_signals.h>
#include <LocoNet.h>
#include "loconet/mrrwa_loconet_adapter.h"
#include "loconet/loconet_txmgr.h"
#include "sensor_interface.h"
#include "pin_switch.h"

using namespace mr_signals;

extern Mrrwa_loconet_adapter loconet;
//extern Logic_collection logic_coll;

bool debug__=false;

const int tx_pin = 47;
const size_t num_sensors = 40;
const size_t tx_buffer_size = 600;

Setup_collection setup_coll(3);
Loop_collection loop_coll(2);

Loconet_txmgr ln_tx_mgr(20, 200, 25000, 3);

Mrrwa_loconet_adapter loconet(setup_coll,loop_coll,LocoNet, tx_pin, num_sensors, tx_buffer_size,ln_tx_mgr);


#include "configs/blackwood_south.cc"

void command_line_loop();

Pin_switch pin1(setup_coll,12);
Pin_switch pin2(setup_coll,13);

Double_switch_head all_sensors_head("sens", pin1, pin2);


int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void check_init_size(const char *term, size_t count, size_t init_size) {


  Serial << term << F(" Count: ") << count << F(" (Init Size: ") <<  init_size << F(")\n");

  if(count > init_size) {
    Serial << F("-More ") << term << F(" defined than initialized; RAM has been inefficiently lost\n");
  }

}

void setup() {
  // put your setup code here, to run once:

 
  setup_coll.execute();

  Serial.begin(57600);
  Serial.println("Blackwood South V1.0");
  Serial.write("free RAM : ");
  Serial.println(freeRam());

  check_init_size("Sensors", loconet.sensor_count(), loconet. sensor_init_size());
  check_init_size("Logics", logic_coll.logic_count(),logic_coll.logic_init_size());
  check_init_size("Setups Funcs",setup_coll.count(), setup_coll.init_size());
  
  Serial << F("\n");

  all_sensors_head.request_aspect(Head_aspect::red);

  ln_tx_mgr.set_slow_duration(millis());
}

void loop() {

  static Runtime_ms last_stat_report = 5000;  // Print first report 5s after startup

 
  loop_coll.execute();
  logic_coll.loop();
  command_line_loop();

  if(millis() > last_stat_report) {
    // Periodically report statistics every 20s

    Serial << F("LocoNet Stats:\n");
    Serial << F("-Tx buffer_high_watermark : ") << loconet.get_buffer_high_watermark() << F("/") << tx_buffer_size << endl;
    Serial << F("-Tx error count : ") << loconet.get_tx_error_count() << endl;
    Serial << F("-LONG_ACKs rcvd : ") << loconet.get_long_ack_count() << endl;
    
    last_stat_report = millis() + 60000;
  }  

  // Check whether we need to change the sensor indicator to green
  if(Head_aspect::red == all_sensors_head.get_aspect()) {
    if(!loconet.any_sensor_indeterminate()) {
      all_sensors_head.request_aspect(Head_aspect::green);  
      ln_tx_mgr.set_slow_duration(millis());
    }    
  }
}

void print_sensor_states(Sensor_interface &sensor) {

  Serial.print(sensor.is_active());
  Serial.print(F(" (ind : "));
  Serial.print(sensor.is_indeterminate());
  Serial.println(")");
}

void print_sensors() {
  Serial.println(F("Print sensor states"));
  loconet.print_sensors();

  Serial.print(F("bkwd_yelta_apb.up_tumbledown(): "));
  print_sensor_states(bkwd_yelta_apb.up_tumbledown());

  Serial.print(F("bkwd_yelta_apb.down_tumbledown(): "));
  print_sensor_states(bkwd_yelta_apb.down_tumbledown());
}

void loconet_on() {
  Serial.println(F("Send LocoNet Power On"));
  loconet.send_opc_gp_on();
}

void loconet_off() {
  Serial.println(F("Send LocoNet Power Off"));
  LocoNet.reportPower(0);
}


void red_heads() {
  Serial.println(F("Send Red Head Aspects"));
  head_1a.request_aspect(Head_aspect::red);
  head_1b.request_aspect(Head_aspect::red);

  head_1107a.request_aspect(Head_aspect::red);
  head_1107b.request_aspect(Head_aspect::red);

  all_sensors_head.request_aspect(Head_aspect::red);
}

void yellow_heads() {
  Serial.println(F("Send Yellow Head Aspects"));  
  head_1a.request_aspect(Head_aspect::yellow);
  head_1b.request_aspect(Head_aspect::yellow); 

  head_1107a.request_aspect(Head_aspect::red);
  head_1107b.request_aspect(Head_aspect::yellow);
  
}

void green_heads() {
  Serial.println(F("Send Green Head Aspects"));  
  head_1a.request_aspect(Head_aspect::green);
  head_1b.request_aspect(Head_aspect::green); 

  head_1107a.request_aspect(Head_aspect::green);
  head_1107b.request_aspect(Head_aspect::red);  

  all_sensors_head.request_aspect(Head_aspect::green);
}

void send_test() {
  Serial.println(F("send_test"));

  lnMsg SendPacket;

  SendPacket.data[0] = 0xB0;
  SendPacket.data[1] = 0x6D;
  SendPacket.data[2] = 0x30;
  
  LocoNet.send(&SendPacket);
}

void debug_toggle() {
  debug__ ^= 1;

  Serial.print("Debug now : ");
  Serial.println(debug__);
}

typedef struct {
  String command;
  String description;
  void (*func)();
} Command_line_entry;

Command_line_entry command_lines[] = {
  { "s",  "Print current loconet sensor state",&print_sensors},
  { "on", "Send LocoNet GP ON", &loconet_on},
  { "off","Send LocoNet GP OFF",&loconet_off},
  { "red","Set select Heads to red",&red_heads},
  { "yel","Set select Heads to yellow", &yellow_heads },
  { "green", "Set select Heads to green", &green_heads },
  { "send_test", "Quick ln send", &send_test },
  { "debug","Toggle debug output", &debug_toggle},

};


void command_line_loop()
{
  String comdata = "";

  if(Serial.available()>0) {

    while (Serial.available() > 0)  
    {     
      char chardata = char(Serial.read());
      // comdata += char(Serial.read());

      if(chardata != '\r' && chardata != '\n') {
        comdata += chardata;
      }
            
      delay(2);
    }
    
    Serial.println(F("received command line:"));
    Serial.println(comdata);
  }

    for(int i=0; i< sizeof(command_lines) / sizeof(command_lines[0]);i++) {
      if(command_lines[i].command == comdata) {
        command_lines[i].func();
      }
    }
}

