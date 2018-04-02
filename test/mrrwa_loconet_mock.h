/*
 * mrrwa_loconet_mock.h
 *
 *  Created on: Mar 3, 2018
 *      Author: ackpu
 */

#ifndef TEST_MRRWA_LOCONET_MOCK_H_
#define TEST_MRRWA_LOCONET_MOCK_H_

#include <stdint.h>

#include "gmock/gmock.h"


namespace mr_signals {

typedef enum
{
    LN_CD_BACKOFF = 0,
    LN_PRIO_BACKOFF,
    LN_NETWORK_BUSY,
    LN_DONE,
    LN_COLLISION,
    LN_UNKNOWN_ERROR,
    LN_RETRY_ERROR
} LN_STATUS ;



struct lnMsg
{
    uint8_t data[16];
};


/*
 * class LocoNetClass
{
  private:
    LnBuf   LnBuffer ;
    void        setTxPin(uint8_t txPin);

  public:
    LocoNetClass();
    void        init(void);
    void        init(uint8_t txPin);
    boolean         available(void);
    uint8_t         length(void);
    lnMsg*      receive(void);
    LN_STATUS   send(lnMsg *TxPacket);
    LN_STATUS   send(lnMsg *TxPacket, uint8_t PrioDelay);
    LN_STATUS   send(uint8_t OpCode, uint8_t Data1, uint8_t Data2);
    LN_STATUS   send(uint8_t OpCode, uint8_t Data1, uint8_t Data2, uint8_t PrioDelay);
    LN_STATUS   sendLongAck(uint8_t ucCode);

    LnBufStats* getStats(void);

    const char* getStatusStr(LN_STATUS Status);

    uint8_t processSwitchSensorMessage( lnMsg *LnPacket ) ;
    uint8_t processPowerTransponderMessage( lnMsg *LnPacket ) ;

    LN_STATUS requestSwitch( uint16_t Address, uint8_t Output, uint8_t Direction ) ;
    LN_STATUS reportSwitch( uint16_t Address ) ;
    LN_STATUS reportSensor( uint16_t Address, uint8_t State ) ;
    LN_STATUS reportPower( uint8_t State ) ;
};
 *
 */
/*
 *             tx_msg.data[0] = dequeue(&ln_tx_queue);

            if(tx_msg.data[0] & DELAY_MSG)
            {
                // This is a delay message
                next_tx_time_ms = (long)(tx_msg.data[0] & 0x3F) + GetTimeMs();
            }
 */

/*
 *     lnMsg SendPacket ;

    int sw2 = 0x00;
    if (!thrown) { sw2 |= OPC_SW_REQ_DIR; }
    if (on) { sw2 |= OPC_SW_REQ_OUT; }
    sw2 |= ((address-1) >> 7) & 0x0F;

    SendPacket.data[ 0 ] = OPC_SW_REQ ;
    SendPacket.data[ 1 ] = (address-1) & 0x7F ;
    SendPacket.data[ 2 ] = sw2 ;
 *
 */

class LocoNetClass {
public:
    virtual void init(uint8_t tx_pin) = 0;

    virtual LN_STATUS reportPower( uint8_t State ) = 0;

    /*
    virtual void reportPower(int pwr) = 0;
    virtual lnMsg* receive() = 0;
    virtual void send(lnMsg*) = 0;
    virtual void processSwitchSensorMessage(lnMsg*) = 0;
    */
    virtual ~LocoNetClass() {}

};

class LocoNetMock : public LocoNetClass {
public:

    MOCK_METHOD1(init,void(uint8_t tx_pin));

    MOCK_METHOD1(reportPower,LN_STATUS(uint8_t State ));

/*
    MOCK_METHOD1(reportPower,void(int pwr));
    MOCK_METHOD0(receive,lnMsg*());
    MOCK_METHOD1(send,void(lnMsg* msg));
    MOCK_METHOD1(processSwitchSensorMessage,void(lnMsg* msg));
*/
};

}



#endif /* TEST_MRRWA_LOCONET_MOCK_H_ */
