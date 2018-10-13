/*
 * mrrwa_loconet_mock.cpp
 *
 *  Created on: Aug 2, 2018
 *      Author: ackpu
 */

#include "mrrwa_loconet_mock.h"


/* Copied from MRRWA Loconet library; used by Loconet Adapter */
uint8_t getLnMsgSize( volatile lnMsg * Msg )
{
    // TODO: Fix in MRRWA library as well!
//    uint8_t len = ( ( Msg->sz.command & (uint8_t)0x60 ) == (uint8_t)0x60 ) ? Msg->sz.mesg_size : ( ( Msg->sz.command & (uint8_t)0x60 ) >> (uint8_t)4 ) + 2;

//    return len > sizeof(lnMsg) ? sizeof(lnMsg) : len;
    return ( ( Msg->sz.command & (uint8_t)0x60 ) == (uint8_t)0x60 ) ? Msg->sz.mesg_size : ( ( Msg->sz.command & (uint8_t)0x60 ) >> (uint8_t)4 ) + 2 ;
}



