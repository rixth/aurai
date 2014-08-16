#include <stdint.h>


#ifndef NRF24L01_h
#define NRF24L01_h

// NRF24L01+ command set

#define NRF24_CMD_R_REGISTER          0 // OR'd with 5 bit address
#define NRF24_CMD_W_REGISTER          _BV(5) // OR'd with 5 bit address
#define NRF24_CMD_R_RX_PAYLOAD        (_BV(0) | _BV(5) | _BV(6))
#define NRF24_CMD_W_TX_PAYLOAD        (_BV(5) | _BV(7))
#define NRF24_CMD_FLUSH_TX            (_BV(0) | _BV(5) | _BV(6) | _BV(7))
#define NRF24_CMD_FLUSH_RX            (_BV(1) | _BV(5) | _BV(6) | _BV(7))
#define NRF24_CMD_REUSE_TX_PL         (_BV(0) | _BV(1) | _BV(5) | _BV(6) | _BV(7))
#define NRF24_CMD_R_RX_PL_WID         (_BV(5) | _BV(6))
#define NRF24_CMD_W_ACK_PAYLOAD       (_BV(3) | _BV(5) | _BV(7)) // OR'd with 3 bit pipe number
#define NRF24_CMD_W_TX_PAYLOAD_NO_ACK (_BV(4) | _BV(5) | _BV(7))
#define NRF24_CMD_NOP                 255

// NRF24L01+ register map

#define NRF24_REG_CONFIG  0x00
#define NRF24_INTERRUPTS  (NRF24_MASK_RX_DR | NRF24_MASK_TX_DS | NRF24_MASK_MAX_RT)
#define NRF24_MASK_RX_DR  _BV(6)
#define NRF24_MASK_TX_DS  _BV(5)
#define NRF24_MASK_MAX_RT _BV(4)
#define NRF24_EN_CRC      _BV(3)
#define NRF24_CRCO        _BV(2)
#define NRF24_PWR_UP      _BV(1)
#define NRF24_PRIM_RX     _BV(0)

#define NRF24_REG_EN_AA 0x01
#define NRF_ENAA_P5     _BV(5)
#define NRF_ENAA_P4     _BV(4)
#define NRF_ENAA_P3     _BV(3)
#define NRF_ENAA_P2     _BV(2)
#define NRF_ENAA_P1     _BV(1)
#define NRF_ENAA_P0     _BV(0)

#define NRF24_REG_EN_RXADDR 0x02
#define NRF24_ERX_P5        _BV(5)
#define NRF24_ERX_P4        _BV(4)
#define NRF24_ERX_P3        _BV(3)
#define NRF24_ERX_P2        _BV(2)
#define NRF24_ERX_P1        _BV(1)
#define NRF24_ERX_P0        _BV(0)

#define NRF24_REG_SETUP_AW 0x03
#define NRF24_AW1          _BV(1)
#define NRF24_AW0          _BV(0)
#define NRF24_AW_3BITS     NRF24_AW0
#define NRF24_AW_4BITS     NRF24_AW1
#define NRF24_AW_5BITS     (NRF24_AW0 | NRF24_AW1)

#define NRF24_REG_SETUP_RETR 0x04
#define NRF24_ARD3          _BV(7)
#define NRF24_ARD2          _BV(6)
#define NRF24_ARD1          _BV(5)
#define NRF24_ARD0          _BV(4)
#define NRF24_ARD_CLEAR     ~(NRF24_ARD0 | NRF24_ARD1 | NRF24_ARD2 | NRF24_ARD3)
#define NRF24_ARC3          _BV(3)
#define NRF24_ARC2          _BV(2)
#define NRF24_ARC1          _BV(1)
#define NRF24_ARC0          _BV(0)
#define NRF24_ARC_CLEAR     ~(NRF24_ARC0 | NRF24_ARC1 | NRF24_ARC2 | NRF24_ARC3)

#define NRF24_REG_RF_CH    0x05

#define NRF24_REG_RF_SETUP 0x06
#define NRF24_CONT_WAVE    _BV(7)
#define NRF24_RF_DR_LOW    _BV(5)
#define NRF24_PLL_LOCK     _BV(4)
#define NRF24_RF_DR_HIGH   _BV(3)
#define NRF24_RF_PWR1      _BV(2)
#define NRF24_RF_PWR0      _BV(1)
#define NRF24_RF_PWR_CLEAR ~(NRF24_RF_PWR0 | NRF24_RF_PWR1)
#define NRF24_RF_PWR_18DBM 0
#define NRF24_RF_PWR_12DBM NRF24_RF_PWR0
#define NRF24_RF_PWR_6DBM  NRF24_RF_PWR1
#define NRF24_RF_PWR_0DBM  (NRF24_RF_PWR0 | NRF24_RF_PWR1)
#define NRF24_RF_DR_CLEAR  ~(NRF24_RF_DR_HIGH | NRF24_RF_DR_LOW)
#define NRF24_RF_DR_1MBPS  0
#define NRF24_RF_DR_2MBPS  NRF24_RF_DR_HIGH
#define NRF24_RF_DR_250KB  NRF24_RF_DR_LOW

#define NRF24_REG_STATUS 0x07
#define NRF24_RX_DR      _BV(6)
#define NRF24_TX_DS      _BV(5)
#define NRF24_MAX_RT     _BV(4)
#define NRF24_ST_CLEAR   ~(NRF24_RX_DR | NRF24_TX_DS | NRF24_MAX_RT)
#define NRF24_RX_P_NO    0x0e
#define NRF24_RX_P_NO2   _BV(3)
#define NRF24_RX_P_NO1   _BV(2)
#define NRF24_RX_P_NO0   _BV(1)
#define NRF24_ST_TX_FULL _BV(0)

#define NRF24_REG_OBSERVE_TX 0x08
#define NRF24_PLOS_CNT       0xf0
#define NRF24_PLOS_CNT3      _BV(7)
#define NRF24_PLOS_CNT2      _BV(6)
#define NRF24_PLOS_CNT1      _BV(5)
#define NRF24_PLOS_CNT0      _BV(4)
#define NRF24_ARC_CNT        0x0f
#define NRF24_ARC_CNT3       _BV(3)
#define NRF24_ARC_CNT2       _BV(2)
#define NRF24_ARC_CNT1       _BV(1)
#define NRF24_ARC_CNT0       _BV(0)

#define NRF24_REG_RPD 0x09
#define NRF24_RPD     _BV(0)

#define NRF24_REG_RX_ADDR_P0 0x0A

#define NRF24_REG_RX_ADDR_P1 0x0B

#define NRF24_REG_RX_ADDR_P2 0x0C

#define NRF24_REG_RX_ADDR_P3 0x0D

#define NRF24_REG_RX_ADDR_P4 0x0E

#define NRF24_REG_RX_ADDR_P5 0x0F

#define NRF24_REG_TX_ADDR 0x10

#define NRF24_REG_RX_PW_P0 0x11

#define NRF24_REG_RX_PW_P1 0x12

#define NRF24_REG_RX_PW_P2 0x13

#define NRF24_REG_RX_PW_P3 0x14

#define NRF24_REG_RX_PW_P4 0x15

#define NRF24_REG_RX_PW_P5 0x16

#define NRF24_REG_FIFO_STATUS 0x17
#define NRF24_TX_REUSE       _BV(6)
#define NRF24_TX_FULL        _BV(5)
#define NRF24_TX_EMPTY       _BV(4)
#define NRF24_RX_FULL        _BV(1)
#define NRF24_RX_EMPTY       _BV(0)

#define NRF24_REG_DYNPD 0x1C
#define NRF24_DPL_P5   _BV(5)
#define NRF24_DPL_P4   _BV(4)
#define NRF24_DPL_P3   _BV(3)
#define NRF24_DPL_P2   _BV(2)
#define NRF24_DPL_P1   _BV(1)
#define NRF24_DPL_P0   _BV(0)

#define NRF24_REG_FEATURE 0x1D
#define NRF24_EN_DPL     _BV(2)
#define NRF24_EN_ACK_PAY _BV(1)
#define NRF24_EN_DYN_ACK _BV(0)

#define NRF24_TIME_PD_TO_SB1  4500
#define NRF24_TIME_SB_TO_RXTX  130
#define NRF24_TIME_CE_PULSE     10

void NRF24_init();
void NRF24_configure();
void NRF24_setTxAddr(const uint8_t* data, uint8_t len);
void NRF24_setRxAddr(const uint8_t* data, uint8_t len);
void NRF24_rxMode();
bool NRF24_send(const uint8_t* val, uint8_t len);
bool NRF24_dataAvailable();

void NRF24_printConfig();
void NRF24_printStatus();
void NRF24_printFifoStatus();
void NRF24_printObserveTx();
void NRF24_printRPD();
void NRF24_printAddresses();
void NRF24_printRegister(uint8_t addr, const char name[]);

void NRF24_getRx(uint8_t *buf, uint8_t len);
void NRF24_setTx(const uint8_t* data, uint8_t len);

uint8_t NRF24_getRegister(uint8_t addr);
void NRF24_getRegister(uint8_t addr, uint8_t *buf, uint8_t len);

void NRF24_setRegister(uint8_t addr, uint8_t val);
void NRF24_setRegisterAnd(uint8_t addr, uint8_t val);
void NRF24_setRegisterOr(uint8_t addr, uint8_t val);
void NRF24_setRegisterAndOr(uint8_t addr, uint8_t andVal, uint8_t orVal);
void NRF24_setRegister(uint8_t addr, const uint8_t* val, uint8_t len);

uint8_t NRF24_read(uint8_t cmd);
void NRF24_read(uint8_t cmd, uint8_t *buf, uint8_t len);

uint8_t NRF24_write(uint8_t cmd);
void NRF24_write(uint8_t cmd, uint8_t val);
void NRF24_write(uint8_t cmd, const uint8_t* val, uint8_t len);

#endif
