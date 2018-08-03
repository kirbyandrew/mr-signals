/*
 * mrrwa_loconet_mock.cpp
 *
 *  Created on: Aug 2, 2018
 *      Author: ackpu
 */

#include "mrrwa_loconet_mock.h"

using namespace mr_signals;

uint8_t getLnMsgSize( volatile lnMsg * Msg )
{
  return ( ( Msg->sz.command & (uint8_t)0x60 ) == (uint8_t)0x60 ) ? Msg->sz.mesg_size : ( ( Msg->sz.command & (uint8_t)0x60 ) >> (uint8_t)4 ) + 2 ;
}



