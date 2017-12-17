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
#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>
#include "I32CTT.h"
#include "I32CTT_Arduino802154Interface.h"

I32CTT_Arduino802154Interface::I32CTT_Arduino802154Interface() {
  this->rx_buffer = new uint8_t[IEEE_802154_MTU];
  memset(this->rx_buffer, 0, sizeof(uint8_t)*IEEE_802154_MTU);
  this->rx_size = 0;
  this->tx_buffer = new uint8_t[IEEE_802154_MTU];
  memset(this->tx_buffer, 0, sizeof(uint8_t)*IEEE_802154_MTU);
  this->frame_buffer = new uint8_t[PSDU_SIZE+1];
  memset(this->frame_buffer, 0, sizeof(uint8_t)*(PSDU_SIZE+1));
  this->tx_size = 0;
  this->phy_status = 0;
  this->cs_pin = 14;
  this->slp_tx_pin = 16;
  this->rst_pin = 17;
  this->irq_pin = 18;
  this->radio_enabled = false;
  this->current_state = 0;
}

I32CTT_Arduino802154Interface::~I32CTT_Arduino802154Interface() {
  delete this->rx_buffer;
  delete this->tx_buffer;
  delete this->frame_buffer;
  this->phy_status = 0;
  this->cs_pin = 14;
  this->slp_tx_pin = 16;
  this->rst_pin = 17;
  this->irq_pin = 18;
  this->radio_enabled = false;
  this->current_state = 0;
  this->channel = C2480;
}

void I32CTT_Arduino802154Interface::set_pan_id(uint16_t pan_id) {
  this->pan_id = pan_id;
}

void I32CTT_Arduino802154Interface::set_short_addr(uint16_t short_addr) {
  this->short_addr = short_addr;
}

void I32CTT_Arduino802154Interface::set_dst_addr(uint16_t dst_addr) {
  this->dst_addr = dst_addr;
}

void I32CTT_Arduino802154Interface::set_channel(IEEE_802154_CHANNEL channel) {
  this->channel = channel;
}

uint8_t I32CTT_Arduino802154Interface::reg_read(uint8_t addr) {
  uint8_t cmd = 0x80 | (0x3F & addr);
  uint8_t result = 0;

  SPI.beginTransaction(this->spi_settings);
  digitalWrite(this->cs_pin, LOW);
  this->phy_status = SPI.transfer(cmd);
  result = SPI.transfer(0x00);
  digitalWrite(this->cs_pin, HIGH);
  return result;
}

void I32CTT_Arduino802154Interface::reg_write(uint8_t addr, uint8_t value) {
  uint8_t cmd = 0xC0 | (0x3F & addr);
  Serial.println(cmd, HEX);
  Serial.println(value, HEX);
  SPI.beginTransaction(this->spi_settings);
  digitalWrite(this->cs_pin, LOW);
  this->phy_status = SPI.transfer(cmd);
  SPI.transfer(value);
  digitalWrite(this->cs_pin, HIGH);
}

void I32CTT_Arduino802154Interface::fb_read(uint8_t *buffer) {
  uint8_t cmd = 0x20;
  uint8_t phr = 0x00;

  SPI.beginTransaction(this->spi_settings);
  digitalWrite(this->cs_pin, LOW);
  this->phy_status = SPI.transfer(cmd);

  phr = SPI.transfer(0x00);

  if(phr>PSDU_SIZE)
    return; //Something went really wrong. Abort.

  buffer[0] = phr;

  for(int i=0;i<phr;i++)
    buffer[i+1] = SPI.transfer(0x00);

  this->lqi = SPI.transfer(0x00);
  this->rx_status_1 = SPI.transfer(0x00);
  this->rx_status_0 = SPI.transfer(0x00);
  digitalWrite(this->cs_pin, HIGH);
}

void I32CTT_Arduino802154Interface::fb_write(uint8_t *buffer) {
  uint8_t cmd = 0x60;
  uint8_t phr = buffer[0];

  if(phr == 0 || phr>PSDU_SIZE)
    return; // Invalid PSDU size

  SPI.beginTransaction(this->spi_settings);
  digitalWrite(this->cs_pin, LOW);
  this->phy_status = SPI.transfer(cmd);
  SPI.transfer(phr);
  
  for(int i=0;i<phr;i++) {
    SPI.transfer(buffer[i+1]);
  }
  digitalWrite(this->cs_pin, HIGH);
}

void I32CTT_Arduino802154Interface::init() {
  uint8_t pn;
  uint8_t vn;
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, HIGH);
  pinMode(slp_tx_pin, OUTPUT);
  digitalWrite(slp_tx_pin, LOW);
  pinMode(rst_pin, OUTPUT);
  digitalWrite(rst_pin, HIGH);
  pinMode(irq_pin, INPUT);
  
  secnum = map(0,1024, 0, 255, analogRead(A8));
  
  SPI.begin();
  this->spi_settings = SPISettings(100000, MSBFIRST, SPI_MODE0);

  pn = reg_read(PART_NUM);
  Serial.println(PART_NUM, HEX);
  vn = reg_read(VERSION_NUM);
  Serial.println(VERSION_NUM, HEX);

  Serial.println("Detecting radio...");
  Serial.print("Part num: ");
  Serial.println(pn, HEX);
  if(pn == 0x0B)
    Serial.println("Part num OK.");
  else
    return; // Wrong part number
    
  Serial.print("Version num: ");
  Serial.println(vn, HEX);
  if(vn == 0x01 || vn == 0x02)
    Serial.println("Version num OK.");
  else
    return; // Wrong version number

  Serial.println("Setting up radio...");

  Serial.println("Enabling dynamic buffer protection...");
  // Enable Dynamic buffer protection
  uint8_t trx_ctrl_2 = reg_read(TRX_CTRL_2);
  trx_ctrl_2 |= RX_SAFE_MODE;
  reg_write(TRX_CTRL_2, trx_ctrl_2);
  
  Serial.println("Setting IRQ Mask...");
  uint8_t irq_mask = IRQ_3_TRX_END; // Reporting only TRX_END
  reg_write(IRQ_MASK, irq_mask);

  Serial.println("Enabling IRQ Pooling and monitoring thru PHY status...");
  // Show IRQ on PHY_STATUS
  uint8_t trx_ctrl_1 = reg_read(TRX_CTRL_1);
  trx_ctrl_1 = PHY_MONITOR_IRQ_STATUS | IRQ_POLLING_EN | (trx_ctrl_1 & SPI_CMD_MODE_MASK);
  trx_ctrl_1 |= TX_AUTO_CRC_ON;
  reg_write(TRX_CTRL_1, trx_ctrl_1);
  
  Serial.println("Clearing interrupts...");
  reg_read(IRQ_STATUS);

  Serial.println("Setting PAN address...");
  reg_write(PAN_ID_1, (uint8_t)(this->pan_id>>8));
  reg_write(PAN_ID_0, (uint8_t)(this->pan_id & 0xFF));

  Serial.print("My PAN: ");
  Serial.print(reg_read(PAN_ID_1), HEX);
  Serial.print(":");
  Serial.println(reg_read(PAN_ID_0), HEX);

  Serial.println("Setting short address...");
  reg_write(SHORT_ADDR_1, (uint8_t)(this->short_addr>>8));
  reg_write(SHORT_ADDR_0, (uint8_t)(this->short_addr & 0xFF));

  Serial.print("My short address: ");
  Serial.print(reg_read(SHORT_ADDR_1), HEX);
  Serial.print(":");
  Serial.println(reg_read(SHORT_ADDR_0), HEX);

  Serial.println("Setting radio channel...");
  uint8_t phy_cc_cca = reg_read(PHY_CC_CCA);
  phy_cc_cca  = this->channel | (phy_cc_cca & PHY_CC_CCA_CHANNEL_MSK);
  reg_write(PHY_CC_CCA, phy_cc_cca);

  Serial.print("My radio channel: ");
  Serial.println(reg_read(PHY_CC_CCA) & 0x1F, HEX);

  Serial.println("Radio initialized...");

  this->radio_enabled = request_state(RX_AACK_ON);

  if(this->radio_enabled)
    Serial.println("Radio ready");
}

void I32CTT_Arduino802154Interface::update_state() {
  this->current_state = reg_read(TRX_STATUS) & TRX_STATE_MSK;
}

uint8_t I32CTT_Arduino802154Interface::wait_for_state(AT86RF233_TRX_STATUS state) {
  uint8_t trx_status = 0;
  uint64_t elapsed_time = millis();
  do {
    trx_status = reg_read(TRX_STATUS) & TRX_STATE_MSK;
    Serial.println(trx_status, HEX);
  } while (trx_status != state && ((millis()-elapsed_time)<1));

  return trx_status == state;
}

uint8_t I32CTT_Arduino802154Interface::request_state(AT86RF233_TRX_STATE state) {
  uint8_t result = false;
  update_state();
  Serial.print("Current state: ");
  Serial.println(this->current_state, HEX);
  Serial.print("Requested state: ");
  Serial.println(state, HEX);

  switch(current_state) {
    case P_ON_S:
      switch(state) {
        case NOP:
          result = true;
          break;
        case TX_START:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TX_START);
          break;
        case FORCE_TRX_OFF:
          result = true;
          reg_write(TRX_STATE, FORCE_TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case FORCE_PLL_ON:
          break;// Cannot force PLL ON on P_ON
        case RX_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          break;
        case TRX_OFF:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case PLL_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          break;
        case PREP_DEEP_SLEEP:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PREP_DEEP_SLEEP);
          result &= wait_for_state(PREP_DEEP_SLEEP_S);
          break;
        case RX_AACK_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          reg_write(TRX_STATE, RX_AACK_ON);
          result &= wait_for_state(RX_AACK_ON_S);
          break;
        case TX_ARET_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TX_ARET_ON);
          result &= wait_for_state(TX_ARET_ON_S);
          break;
      };
      break;
    case BUSY_RX_S:
      break; // do nothing
    case BUSY_TX_S:
      break; // do nothing
    case RX_ON_S:
      switch(state) {
        case NOP:
          result = true;
          break;
        case TX_START:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TX_START);
          break;
        case FORCE_TRX_OFF:
          result = true;
          reg_write(TRX_STATE, FORCE_TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case FORCE_PLL_ON:
          result = true;
          reg_write(TRX_STATE, FORCE_PLL_ON);
          result &= wait_for_state(PLL_ON_S);
        case RX_ON:
          result = true; // Already here
          break;
        case TRX_OFF:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case PLL_ON:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          break;
        case PREP_DEEP_SLEEP:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PREP_DEEP_SLEEP);
          result &= wait_for_state(PREP_DEEP_SLEEP_S);
          break;
        case RX_AACK_ON:
          result = true;
          reg_write(TRX_STATE, RX_AACK_ON);
          result &= wait_for_state(RX_AACK_ON_S);
          break;
        case TX_ARET_ON:
          result = true;
          reg_write(TRX_STATE, TX_ARET_ON);
          result &= wait_for_state(TX_ARET_ON_S);
          break;
      };
      break;
    case TRX_OFF_S:
      switch(state) {
        case NOP:
          result = true;
          break;
        case TX_START:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TX_START);
          break;
        case FORCE_TRX_OFF:
          result = true;
          break; // Already here
        case FORCE_PLL_ON:
          result = true;
          reg_write(TRX_STATE, FORCE_PLL_ON);
          result &= wait_for_state(PLL_ON_S);
        case RX_ON:
          result = true;
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          break;
        case TRX_OFF:
          result = true; // already here
          break;
        case PLL_ON:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          break;
        case PREP_DEEP_SLEEP:
          result = true;
          reg_write(TRX_STATE, PREP_DEEP_SLEEP);
          result &= wait_for_state(PREP_DEEP_SLEEP_S);
          break;
        case RX_AACK_ON:
          result = true;
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          reg_write(TRX_STATE, RX_AACK_ON);
          result &= wait_for_state(RX_AACK_ON_S);
          break;
        case TX_ARET_ON:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TX_ARET_ON);
          result &= wait_for_state(TX_ARET_ON_S);
          break;
      };
      break;
    case PLL_ON_S:
      switch(state) {
        case NOP:
          result = true;
          break;
        case TX_START:
          result = true;
          reg_write(TRX_STATE, TX_START);
          break;
        case FORCE_TRX_OFF:
          result = true;
          reg_write(TRX_STATE, FORCE_TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case FORCE_PLL_ON:
          result = true;
          break; // Already here
        case RX_ON:
          result = true;
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          break;
        case TRX_OFF:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case PLL_ON:
          result = true; // already here
          break;
        case PREP_DEEP_SLEEP:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PREP_DEEP_SLEEP);
          result &= wait_for_state(PREP_DEEP_SLEEP_S);
          break;
        case RX_AACK_ON:
          result = true;
          reg_write(TRX_STATE, RX_AACK_ON);
          result &= wait_for_state(RX_AACK_ON_S);
          break;
        case TX_ARET_ON:
          result = true;
          reg_write(TRX_STATE, TX_ARET_ON);
          result &= wait_for_state(TX_ARET_ON_S);
          break;
      };
      break;
    case SLEEP_S:
      result = true;
      reg_write(TRX_STATE, FORCE_TRX_OFF);
      result &= wait_for_state(TRX_OFF_S);
      break; // do nothing.
    case PREP_DEEP_SLEEP_S:
      switch(state) {
        case NOP:
          result = true;
          break;
        case TX_START:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TX_START);
          break;
        case FORCE_TRX_OFF:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case FORCE_PLL_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
        case RX_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          break;
        case TRX_OFF:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case PLL_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          break;
        case PREP_DEEP_SLEEP:
          result = true;  // already here
          break;
        case RX_AACK_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          reg_write(TRX_STATE, RX_AACK_ON);
          result &= wait_for_state(RX_AACK_ON_S);
          break;
        case TX_ARET_ON:
          result = true;
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TX_ARET_ON);
          result &= wait_for_state(TX_ARET_ON_S);
          break;
      };
      break;
    case BUSY_RX_AACK_S:
      break; // do nothing
    case BUSY_TX_ARET_S:
      break; // do nothing
    case RX_AACK_ON_S:
      switch(state) {
        case NOP:
          result = true;
          break;
        case TX_START:
          result = true;
          reg_write(TRX_STATE, TX_ARET_ON);
          result &= wait_for_state(TX_ARET_ON_S);
          reg_write(TRX_STATE, TX_START);
          break;
        case FORCE_TRX_OFF:
          result = true;
          reg_write(TRX_STATE, FORCE_TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case FORCE_PLL_ON:
          result = true;
          reg_write(TRX_STATE, FORCE_PLL_ON);
          result &= wait_for_state(PLL_ON_S);
        case RX_ON:
          result = true;
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          break;
        case TRX_OFF:
          result = true;
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case PLL_ON:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          break;
        case PREP_DEEP_SLEEP:
          result = true;
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PREP_DEEP_SLEEP);
          result &= wait_for_state(PREP_DEEP_SLEEP_S);
          break;
        case RX_AACK_ON:
          result = true; // Already here
          break;
        case TX_ARET_ON:
          result = true;
          reg_write(TRX_STATE, TX_ARET_ON);
          result &= wait_for_state(TX_ARET_ON_S);
          break;
      };
      break;
    case TX_ARET_ON_S:
      switch(state) {
        case NOP:
          result = true;
          break;
        case TX_START:
          result = true;
          reg_write(TRX_STATE, TX_START);
          break;
        case FORCE_TRX_OFF:
          result = true;
          reg_write(TRX_STATE, FORCE_TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case FORCE_PLL_ON:
          result = true;
          reg_write(TRX_STATE, FORCE_PLL_ON);
          result &= wait_for_state(PLL_ON_S);
        case RX_ON:
          result = true;
          reg_write(TRX_STATE, RX_ON);
          result &= wait_for_state(RX_ON_S);
          break;
        case TRX_OFF:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          break;
        case PLL_ON:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          break;
        case PREP_DEEP_SLEEP:
          result = true;
          reg_write(TRX_STATE, PLL_ON);
          result &= wait_for_state(PLL_ON_S);
          reg_write(TRX_STATE, TRX_OFF);
          result &= wait_for_state(TRX_OFF_S);
          reg_write(TRX_STATE, PREP_DEEP_SLEEP);
          result &= wait_for_state(PREP_DEEP_SLEEP_S);
          break;
        case RX_AACK_ON:
          result = true;
          reg_write(TRX_STATE, RX_AACK_ON);
          result &= wait_for_state(RX_AACK_ON_S);
          break;
        case TX_ARET_ON:
          result = true; // Already here
          break;
      };
      break;
    case STATE_TRANSITION_IN_PROGRESS_S:
      break; // do nothing
    default:
      result = true;
      reg_write(TRX_STATE, FORCE_TRX_OFF);
      result &= wait_for_state(TRX_OFF_S);
  };

  return result;
}

void I32CTT_Arduino802154Interface::update() {
  uint8_t phr = 0;
  uint8_t trx_status;
  uint8_t trac_status;
  char str_fmt[3];
  // Get current status to update IRQ status on PHY_STATUS
  update_state();
  trx_status = reg_read(TRX_STATE);

  switch(current_state) {
    case TX_ARET_ON_S:
      //Serial.println("Trying to send...");
      // A frame transmission was successfully completed
      if(this->phy_status & IRQ_3_TRX_END ||
        (millis()-this->last_try)>TX_POLL_TIMEOUT
      ) {
        if((millis()-this->last_try)>TX_POLL_TIMEOUT ) {
          Serial.println("Packet timed out");
        } else {
          trac_status = trx_status>>5;
          switch(trac_status) {
            case TRAC_SUCCESS:
                Serial.println("Packet sent");
              break;
            case TRAC_SUCCESS_DATA_PENDING:
                Serial.println("Data pending");
              break;
            case TRAC_CHANNEL_ACCESS_FAILURE:
                Serial.println("Access Failure");
              break;
            case TRAC_NO_ACK:
                Serial.println("No ACK");
              break;
            case TRAC_INVALID:
                Serial.println("Invalid");
              break;
          };
        }
        this->tx_size  = 0;
        this->package_queued = false;
        reg_read(IRQ_STATUS); // Clear interrupt status
        request_state(RX_AACK_ON); // Request listen state
      }
      break;
    case RX_AACK_ON_S:
      // A frame reception was successfully completed
      if(this->phy_status & IRQ_3_TRX_END) {
        IEEE_802154_FRAME_FCF response_fcf;

        fb_read(this->frame_buffer);

        phr = this->frame_buffer[0];

        memcpy(&response_fcf, this->frame_buffer+1, sizeof(IEEE_802154_FRAME_FCF));


        for(int i=1;i<(phr+1);i++) {
          sprintf(str_fmt, "%02x", this->frame_buffer[i]);
          Serial.print(str_fmt);
          Serial.print(":");
        }
        Serial.print("\r\n");

        // Fill response buffer only if it makes sense
        if(
          response_fcf.sec_enabled == SEC_DISABLED &&
          response_fcf.pan_id_comp == PAN_ID_COMPRESSION &&
          response_fcf.dst_addr_mode == SHORT_ADDR &&
          response_fcf.src_addr_mode == SHORT_ADDR
        ) {
          this->rx_size = phr-11;
          memcpy(this->rx_buffer, this->frame_buffer+10, this->rx_size);
          this->d_available = true;
        }
        reg_read(IRQ_STATUS); // Clear interrupt status
      }
      break;
  }
}

uint8_t I32CTT_Arduino802154Interface::available() {
  uint8_t result = 0;
  update_state();
  if (
    this->radio_enabled && // Radio initialized
    ( // Not busy states
      this->current_state == RX_ON_S ||
      this->current_state == PLL_ON_S ||
      this->current_state == RX_AACK_ON_S ||
      this->current_state == TX_ARET_ON_S
    ) &
    !this->package_queued
  ) {
    result = 1;
  }
  return result;
}

uint8_t I32CTT_Arduino802154Interface::data_available() {
  uint8_t result = this->d_available;
  this->d_available = false; // Clear data available after read
  return result;
}

void I32CTT_Arduino802154Interface::send() {
  char str_fmt[3];
  update(); // try to update before send.
  
  uint8_t frame_pos;
  if(this->tx_size == 0)
    return; // Nothing to do.

  if(available()) {
    frame_pos = 0;

    secnum++;

    IEEE_802154_FRAME_FCF fcf;

    fcf.frame_type = DATA;
    fcf.sec_enabled = SEC_DISABLED;
    fcf.frame_pending = NOT_PENDING_FRAME;
    fcf.ack_request = ACK_ENABLED;
    fcf.pan_id_comp = PAN_ID_COMPRESSION;
    fcf.res_0 = 0x000;
    fcf.dst_addr_mode = SHORT_ADDR;
    fcf.frame_ver = IEEE_802154_2006;
    fcf.src_addr_mode = SHORT_ADDR;

    request_state(TX_ARET_ON);

    frame_pos += sizeof(uint8_t); // Space for PHR

    memcpy(frame_buffer+frame_pos, &fcf, sizeof(IEEE_802154_FRAME_FCF));
    frame_pos += sizeof(IEEE_802154_FRAME_FCF); // FCF

    memcpy(frame_buffer+frame_pos, &secnum, sizeof(uint8_t));
    frame_pos += sizeof(uint8_t); // Sequence Number

    memcpy(frame_buffer+frame_pos, &pan_id, sizeof(uint16_t));
    frame_pos += sizeof(uint16_t); // Source PAN ID
    memcpy(frame_buffer+frame_pos, &dst_addr, sizeof(uint16_t));
    frame_pos += sizeof(uint16_t); // Source short address
    memcpy(frame_buffer+frame_pos, &short_addr, sizeof(uint16_t));
    frame_pos += sizeof(uint16_t); // Destination short address

    memcpy(frame_buffer+frame_pos, this->tx_buffer, this->tx_size);
    frame_pos += this->tx_size;
    memset(frame_buffer+frame_pos, 0, sizeof(uint16_t));
    frame_pos += sizeof(uint16_t);

    this->frame_buffer[0] = frame_pos-1; // Set PHR size
    Serial.println("BEGIN: Buffer sizes");
    Serial.println(this->frame_buffer[0], DEC);
    Serial.println(this->tx_size, DEC);
    Serial.println("END: Buffer sizes");

    for(int i=1;i<(this->frame_buffer[0]+1);i++) {
      sprintf(str_fmt, "%02x", this->frame_buffer[i]);
      Serial.print(str_fmt);
      Serial.print(":");
    }
    Serial.print("\r\n");

    Serial.print("Sequence: ");
    Serial.println(secnum, DEC);
    this->last_try = millis();
    this->package_queued = true;
    fb_write(this->frame_buffer);
    request_state(TX_START);
  }
}

uint16_t I32CTT_Arduino802154Interface::get_MTU() {
  // Do nothing
  return IEEE_802154_MTU;
}