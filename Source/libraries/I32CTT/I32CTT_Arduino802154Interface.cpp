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
  this->rx_buffer = (uint8_t*)malloc(sizeof(uint8_t)*IEEE_802154_MTU);
  memset(this->rx_buffer, 0, sizeof(uint8_t)*IEEE_802154_MTU);
  this->rx_size = 0;
  this->tx_buffer = (uint8_t*)malloc(sizeof(uint8_t)*IEEE_802154_MTU);
  memset(this->tx_buffer, 0, sizeof(uint8_t)*IEEE_802154_MTU);
  this->tx_size = 0;
}

I32CTT_Arduino802154Interface::~I32CTT_Arduino802154Interface() {
  delete this->rx_buffer;
  delete this->tx_buffer;
}

void I32CTT_Arduino802154Interface::init() {
  uint8_t pn;
  uint8_t vn;
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
  
  digitalWrite(14, LOW);
  SPI.transfer(0x80 | 0x1C);
  pn = SPI.transfer(0x00);
  digitalWrite(14, HIGH);
  
  digitalWrite(14, LOW);
  SPI.transfer(0x80 | 0x1D);
  vn = SPI.transfer(0x00);
  digitalWrite(14, HIGH);

  Serial.println("802.15.4 radio initilized...");
  Serial.print("Part num: ");
  Serial.println(pn, HEX);
  if(pn == 0x0B)
    Serial.println("Part num OK.");
  Serial.print("Version num: ");
  Serial.println(vn, HEX);
  if(vn == 0x01 | vn == 0x02)
    Serial.println("Version num OK.");
}

void I32CTT_Arduino802154Interface::update() {
  // Do nothing
}

uint8_t I32CTT_Arduino802154Interface::available() {
  // Do nothing
  return 1;
}

uint8_t I32CTT_Arduino802154Interface::data_available() {
  // Do nothing
  return 0;
}

void I32CTT_Arduino802154Interface::send() {
  // Do nothing
  this->tx_size = 0;
  //Serial.println("Sending...");
}

uint16_t I32CTT_Arduino802154Interface::get_MTU() {
  // Do nothing
  return IEEE_802154_MTU;
}