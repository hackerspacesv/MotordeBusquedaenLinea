#ifndef HS2VT2_H
#define HS2VT2_H

#define HS2VT2_MAX_MAC_PHY 102
#define MAX_MODE_COUNT 64

enum CMD_t {
  CMD_R  = 1,
  CMD_AR = 2,
  CMD_W  = 3,
  CMD_AW = 4
};

class HS2VT2_Interface {
  public:
    uint8_t rx_buffer[HS2VT2_MAX_MAC_PHY];
    uint8_t rx_size;
    uint8_t tx_buffer[HS2VT2_MAX_MAC_PHY];
    uint8_t tx_size;
    virtual void init()=0;
    virtual void update()=0;
    virtual uint8_t available()=0;
    virtual void send()=0;
};

class HS2VT2_ModeDriver {
  public:
    HS2VT2_ModeDriver(uint32_t mode_id);
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

class HS2VT2_Controller {
  public:
    HS2VT2_Controller();
    uint8_t set_interface(HS2VT2_Interface &iface);
    uint8_t add_mode_driver(HS2VT2_ModeDriver &drv);
    void init();
    void run();
  private:
    void parse(uint8_t *buffer, uint8_t buffsize);
    uint8_t valid_size(uint8_t cmd_type, uint8_t buffsize);
    uint8_t reg_count(uint8_t cmd_type, uint8_t buffsize);
    uint32_t get_reg(uint8_t *buffer, uint8_t cmd_type, uint8_t pos);
    uint32_t get_data(uint8_t *buffer, uint8_t cmd_type, uint8_t pos);
    void put_reg(uint8_t *buffer, uint32_t reg, uint8_t cmd_type, uint8_t pos);
    void put_data(uint8_t *buffer, uint32_t data, uint8_t cmd_type, uint8_t pos);
    HS2VT2_ModeDriver *drivers[MAX_MODE_COUNT];
    HS2VT2_Interface *interface;
    uint8_t mode_counter;
    uint8_t current_mode;
};
#endif
