/*
 * 
 * This file is part of I32CTT (Integer 32-bit Control & Telemetry Transport).
 * Copyright (C) 2017 Mario Gomez.
 * 
 * I32CTT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * I32CTT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with I32CTT.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef I32CTT_Arduino802154Interface_H
#define I32CTT_Arduino802154Interface_H

#ifdef ARDUINO
#define IEEE_802154_MTU 106
#define PSDU_SIZE 127

#ifndef SPI_H
#include <SPI.h>
#endif

#define TRX_STATE_MSK          0x1F
#define RX_SAFE_MODE           (1<<7)
#define SPI_CMD_MODE_MASK      0xF3
#define PHY_CC_CCA_CHANNEL_MSK 0xE0
#define PHY_MONITOR_DEFAULT    0
#define PHY_MONITOR_TRX_STATUS 1<<2
#define PHY_MONITOR_PHY_RSSI   2<<2
#define PHY_MONITOR_IRQ_STATUS 3<<2

enum IEEE_802154_FRAME_TYPE {
  BEACON = 0,
  DATA,
  ACK,
  MAC_CMD
};

enum IEEE_802154_ADDR_MODE {
  PANID_ADDR_NOT_PRES = 0,
  RES,
  SHORT_ADDR,
  EXT_ADDR
};

enum IEEE_802154_FRAME_VER {
  IEEE_802154_2003 = 0,
  IEEE_802154_2006
};

struct IEEE_802154_FRAME {
  uint frame_type : 3;
  uint sec_enabled : 1;
  uint frame_pending : 1;
  uint ack_request : 1;
  uint pan_id_comp : 1;
  uint res_0 : 3;
  uint dst_addr_mode : 2;
  uint frame_ver : 2;
  uint src_addr_mode : 2;
  uint8_t sec_num;
};

enum AT86RF233_TRX_STATUS {
  P_ON_S = 0x00,
  BUSY_RX_S,
  BUSY_TX_S,
  RX_ON_S = 0x06,
  TRX_OFF_S = 0x08,
  PLL_ON_S,
  SLEEP_S = 0x0F,
  PREP_DEEP_SLEEP_S,
  BUSY_RX_AACK_S,
  BUSY_TX_ARET_S,
  RX_AACK_ON_S = 0x16,
  TX_ARET_ON_S = 0x19,
  STATE_TRANSITION_IN_PROGRESS_S = 0x1F
};

enum AT86RF233_TRX_STATE {
  NOP = 0x00,
  TX_START = 0x02,
  FORCE_TRX_OFF,
  FORCE_PLL_ON,
  RX_ON = 0x06,
  TRX_OFF = 0x08,
  PLL_ON,
  PREP_DEEP_SLEEP = 0x10,
  RX_AACK_ON = 0x16,
  TX_ARET_ON = 0x19
};

enum IEEE_802154_CHANNEL {
  C2405 = 0x0B,
  C2410,
  C2415,
  C2420,
  C2425,
  C2430,
  C2435,
  C2440,
  C2445,
  C2450,
  C2455,
  C2460,
  C2465,
  C2470,
  C2475,
  C2480
};

enum AT86RF233_REGISTERS {
  TRX_STATUS = 0x01,
  TRX_STATE,
  TRX_CTRL_0,
  TRX_CTRL_1,
  PHY_TX_PWR,
  PHY_RSSI,
  PHY_ED_LEVEL,
  PHY_CC_CCA,
  CCA_THRES,
  RX_CTRL,
  SFD_VALUE,
  TRX_CTRL_2,
  ANT_DIV,
  IRQ_MASK,
  IRQ_STATUS,
  VREG_CTRL,
  BATMON,
  XOSC_CTRL,
  CC_CTRL_O,
  CC_CTRL_1,
  RX_SYN,
  TRX_RPC,
  XAH_CTRL_1,
  FTN_CTRL,
  XAH_CTRL_2,
  PLL_CF,
  PLL_DCU,
  PART_NUM,
  VERSION_NUM,
  MAN_ID_0,
  MAN_ID_1,
  SHORT_ADDR_0,
  SHORT_ADDR_1,
  PAN_ID_0,
  PAN_ID_1,
  IEEE_ADDR_0,
  IEEE_ADDR_1,
  IEEE_ADDR_2,
  IEEE_ADDR_3,
  IEEE_ADDR_4,
  IEEE_ADDR_5,
  IEEE_ADDR_6,
  IEEE_ADDR_7,
  XAH_CTRL_0,
  CSMA_SEED_O,
  CSMA_SEED_1,
  CSMA_BE,
  TST_CTRL_DIGI = 0x36,
  TST_AGC = 0x3C,
  TST_SDM = 0x3D
};

class I32CTT_Arduino802154Interface: public I32CTT_Interface {
  public:
    I32CTT_Arduino802154Interface();
    ~I32CTT_Arduino802154Interface();
    void set_pan_id(uint16_t pan_id);
    void set_short_addr(uint16_t short_addr);
    void set_channel(IEEE_802154_CHANNEL channel);
    void init();
    void update();
    uint8_t available();
    uint8_t data_available();
    void send();
    uint16_t get_MTU();
  private:
    uint8_t *frame_buffer;
    uint8_t reg_read(uint8_t addr);
    void reg_write(uint8_t addr, uint8_t value);
    void fb_read(uint8_t *buffer);
    void fb_write(uint8_t *buffer);
    uint8_t request_state(AT86RF233_TRX_STATE state);
    uint8_t wait_for_state(AT86RF233_TRX_STATUS state);
    void update_state();
    uint8_t phy_status;
    uint8_t cs_pin;
    uint8_t slp_tx_pin;
    uint8_t rst_pin;
    uint8_t irq_pin;
    uint8_t lqi;
    uint8_t rx_status_0;
    uint8_t rx_status_1;
    uint8_t radio_enabled;
    uint8_t current_state;
    uint16_t short_addr;
    uint16_t pan_id;
    uint8_t channel;
    SPISettings spi_settings;
};

#endif

#endif
