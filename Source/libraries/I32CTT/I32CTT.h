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

/*
 * Acerca de este archivo: Este sketch es la implementación de referencia
 * para el protocolo de enteros de 32 bits para control y transporte de
 * telemetría (I32CTT).
 */
#ifndef I32CTT_H
#define I32CTT_H

#define I32CTT_MAX_MAC_PHY 102
#define MAX_MODE_COUNT 64

// TODO: Check for memory leaks

enum CMD_t {
  CMD_R   = 1,
  CMD_AR  = 2,
  CMD_W   = 3,
  CMD_AW  = 4,
  CMD_ENA = 5,
  CMD_DIS = 6
};

struct __attribute__((__packed__)) I32CTT_Header {
  uint8_t cmd;
  uint8_t mode;
};

struct __attribute__((__packed__)) I32CTT_RegData {
  uint16_t reg;
  uint32_t data;
};

struct __attribute__((__packed__)) I32CTT_Reg {
  uint16_t reg;
};

class I32CTT_Interface {
  public:
    uint8_t rx_buffer[I32CTT_MAX_MAC_PHY];
    uint8_t rx_size;
    uint8_t tx_buffer[I32CTT_MAX_MAC_PHY];
    uint8_t tx_size;
    virtual void init()=0;
    virtual void update()=0;
    virtual uint8_t available()=0;
    virtual void send()=0;
};

class I32CTT_ModeDriver {
  public:
    I32CTT_ModeDriver(uint32_t driver_name);
    virtual void init()=0;
    virtual uint32_t read(uint16_t addr)=0;
    virtual uint16_t write(uint16_t addr, uint32_t data)=0;
    virtual void update()=0;
    virtual uint8_t enabled() = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    static uint32_t str2name(const char *str);
    uint32_t get_name();
  protected:
    uint32_t driver_name;
};

class I32CTT_Controller {
  public:
    I32CTT_Controller(uint8_t total_modes);
    uint8_t set_interface(I32CTT_Interface &iface);
    uint8_t add_mode_driver(I32CTT_ModeDriver &drv);
    void init();
    void run();
    void enable_scheduler();
    void disable_scheduler();
    static uint16_t get_reg(uint8_t *buffer, uint8_t cmd_type, uint8_t pos);
    static uint32_t get_data(uint8_t *buffer, uint8_t cmd_type, uint8_t pos);
    static void put_reg(uint8_t *buffer, uint16_t reg, uint8_t cmd_type, uint8_t pos);
    static void put_data(uint8_t *buffer, uint32_t data, uint8_t cmd_type, uint8_t pos);
    static uint8_t reg_count(uint8_t cmd_type, uint8_t buffsize);
  private:
    void parse(uint8_t *buffer, uint8_t buffsize);
    uint8_t valid_size(uint8_t cmd_type, uint8_t buffsize);
    I32CTT_ModeDriver **drivers;
    I32CTT_Interface *interface;
    uint8_t total_modes;
    uint8_t modes_set;
    uint8_t scheduler_enabled;
};
#endif
