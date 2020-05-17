
#include <StandardCplusplus.h>


#include <mr_signals.h>
#include <LocoNet.h>
#include "loconet/mrrwa_loconet_adapter.h"

/*
#include "logic_collection.h"

#include "configs/blackwood_south.h"  // Update this as neccessary; must follow mr_signals.h
*/

using namespace mr_signals;

extern Mrrwa_loconet_adapter loconet;
//extern Logic_collection logic_collection;

const int tx_pin = 47;
const size_t num_sensors = 10;
const size_t tx_buffer_size = 6;

Mrrwa_loconet_adapter loconet(LocoNet, tx_pin, num_sensors, tx_buffer_size);


#include "configs/blackwood_south.cc"

void command_line_loop();


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



 // LocoNet.init(47);
  loconet.setup();

  Serial.begin(57600);
  Serial.println("Blackwood South V1.0");
  Serial.write("free RAM : ");
  Serial.println(freeRam());

  check_init_size("Sensors", loconet.sensor_count(), loconet. sensor_init_size());
  check_init_size("Logics", logic_collection.logic_count(),logic_collection.logic_init_size());

  Serial << F("\n");
}

void loop() {

  loconet.loop();
  logic_collection.loop();
  command_line_loop();
  
}

void print_sensors() {
  Serial.println(F("Print sensor states"));
  loconet.print_sensors();
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
}

void send_test() {
  Serial.println(F("send_test"));

  lnMsg SendPacket;

  SendPacket.data[0] = 0xB0;
  SendPacket.data[1] = 0x6D;
  SendPacket.data[2] = 0x30;
  
  LocoNet.send(&SendPacket);
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
/*
    if (comdata == "sensors" || comdata == "s")
    {
      loconet.PrintSensors();
    }
    else if (comdata == "masts")
    {
      mast_list.PrintMasts();
    }
    else if(comdata == "on")
    {
     loconet.SendOpcGpOn();
    }
    else if(comdata == "loconet")
    {
      loconet.DebugPrint();
    }
    else if(comdata == "r")
    {
      head_3c.SetAspect(headAspect::red);
      head_5a.SetAspect(headAspect::red);
      head_5b.SetAspect(headAspect::red);
      head_24b.SetAspect(headAspect::red);
      head_24c.SetAspect(headAspect::red);
    }
    else if(comdata == "y")
    {
      head_3c.SetAspect(headAspect::yellow);
      head_5a.SetAspect(headAspect::yellow);
      head_5b.SetAspect(headAspect::yellow);
      head_24b.SetAspect(headAspect::yellow);
      head_24c.SetAspect(headAspect::yellow);
    }
    else if(comdata == "g")
    {
      head_3c.SetAspect(headAspect::green);
      head_5a.SetAspect(headAspect::green);
      head_5b.SetAspect(headAspect::green);
      head_24b.SetAspect(headAspect::green);
      head_24c.SetAspect(headAspect::green);
    }
  }
*/  
}

