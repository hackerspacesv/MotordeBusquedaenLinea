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

enum CMD_t {
  CMD_R  = 1,
  CMD_AR = 2,
  CMD_W  = 3,
  CMD_AW = 4
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
    I32CTT_ModeDriver(uint32_t mode_id);
    virtual uint32_t read(uint8_t addr)=0;
    virtual uint8_t write(uint8_t addr, uint32_t data)=0;
    virtual void h_answer_read(uint8_t addr, uint32_t data)=0;
    virtual void h_answer_write(uint8_t addr)=0;
    virtual void update()=0;
    virtual void on_mode_entry()=0;
    virtual void on_mode_exit()=0;
  protected:
    uint32_t mode_id;
};

class I32CTT_Controller {
  public:
    I32CTT_Controller();
    uint8_t set_interface(I32CTT_Interface &iface);
    uint8_t add_mode_driver(I32CTT_ModeDriver &drv);
    void init();
    void run();
    static uint32_t get_reg(uint8_t *buffer, uint8_t cmd_type, uint8_t pos);
    static uint32_t get_data(uint8_t *buffer, uint8_t cmd_type, uint8_t pos);
    static uint8_t reg_count(uint8_t cmd_type, uint8_t buffsize);
  private:
    void parse(uint8_t *buffer, uint8_t buffsize);
    uint8_t valid_size(uint8_t cmd_type, uint8_t buffsize);
    void put_reg(uint8_t *buffer, uint32_t reg, uint8_t cmd_type, uint8_t pos);
    void put_data(uint8_t *buffer, uint32_t data, uint8_t cmd_type, uint8_t pos);
    I32CTT_ModeDriver *drivers[MAX_MODE_COUNT];
    I32CTT_Interface *interface;
    uint8_t mode_counter;
    uint8_t current_mode;
};
#endif
