/*
 * 
 * This file is part of I32CTT (Integer 32-bit Control & Telemetry Transport).
 * Copyright (C) 2017 Mario Gomez.
 * 
 * I32CTT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * pwm_val_m1a
 * I32CTT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with I32CTT.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include "I32CTT.h"
#include "MBL_ManualDriver.h"
#include "Arduino.h"

MBL_ManualDriver::MBL_ManualDriver(uint32_t mode_id) : I32CTT_ModeDriver(mode_id) {
  // Do nothing. Just call superclass parent
}
 
uint32_t MBL_ManualDriver::read(uint16_t addr) {
  uint32_t result;
  switch(addr) {
    case 0x0000:
      result = this->driver_name;
      break;
    default:
      result = 0x00;
  }
  return result; // Test Pattern
}

uint16_t MBL_ManualDriver::write(uint16_t addr, uint32_t data) {
  switch(addr) {
    case 0x0001: // X
      this->x = data;
      break;
    case 0x0002: // Y
      this->y = data;
      break;
    default:
      break;
  }
  return addr; // Null driver always return a valid address
}
  uint16_t result;

void MBL_ManualDriver::init() {
  // Set pin modes.
  pinMode(MBL_M1A, OUTPUT);
  digitalWrite(MBL_M1A, LOW);
  pinMode(MBL_M1B, OUTPUT);
  digitalWrite(MBL_M1B, LOW);
  pinMode(MBL_M2A, OUTPUT);
  digitalWrite(MBL_M2A, LOW);
  pinMode(MBL_M2B, OUTPUT);
  digitalWrite(MBL_M2B, LOW);
}

uint32_t MBL_ManualDriver::fp_ABS(uint32_t val) {
  uint32_t result = 0;
  if((val&0x80000000)>0) {
    result = (!val+1) & 0xEFFFFFFF;
  } else{
    result = val;
  }
  return result;
}

void MBL_ManualDriver::update() {
  uint8_t pwm_val_m1a, pwm_val_m1b, pwm_val_m2a, pwm_val_m2b;

  // Nota: Este control manual utiliza logica difusa
  // para traducir posiciones del mando X y Y a valores
  // de PWM en los motores.
  uint32_t power1 = fp_ABS(this->y)*(this->y*(0x10000-fp_ABS(this->x))+this->x);
  uint32_t power2 = fp_ABS(this->y)*(this->y*(0x10000-fp_ABS(this->x))+this->x*0xFFFF0000);
  
  pwm_val_m1a =((power1&0x80000000)==0 && power1>0) ? 0 : (0xFF0000*fp_ABS(power1))>>16;
  pwm_val_m1b =((power1&0x80000000)>0 && (power1&0xEFFFFFFF)>=0) ? 0 : (0xFF0000*fp_ABS(power1))>>16;
  pwm_val_m2a =((power2&0x80000000)==0 && power2>0) ? 0 : (0xFF0000*fp_ABS(power2))>>16;
  pwm_val_m2b =((power2&0x80000000)>0 && (power2&0xEFFFFFFF)>=0) ? 0 : (0xFF0000*fp_ABS(power2))>>16;

  analogWrite(MBL_M1A, pwm_val_m1a);
  analogWrite(MBL_M1B, pwm_val_m1b);
  analogWrite(MBL_M2A, pwm_val_m2a);
  analogWrite(MBL_M2B, pwm_val_m2b);
}

uint8_t MBL_ManualDriver::enabled() {
  return this->is_enabled; // Driver is always enabled
}

void MBL_ManualDriver::enable() {
  this->is_enabled = 1;
}

void MBL_ManualDriver::disable() {
  this->is_enabled = 0;
}

