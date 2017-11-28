#define HS2VT2_MAX_MAC_PHY 102

class HS2VT2_Interface {
  public:
    uint8_t rx_buffer[HS2VT2_MAX_MAC_PHY];
    uint8_t rx_size;
    uint8_t tx_buffer[HS2VT2_MAX_MAC_PHY];
    uint8_t tx_size;
    virtual void init();
    virtual void update();
    virtual uint8_t available();
    virtual void send();
};

class HS2VT2_ModeDriver {
  public:
    HS2VT2_ModeDriver(uint32_t mode_id);
    virtual uint32_t read(uint8_t addr);
    virtual uint8_t write(uint8_t addr, uint32_t data);
    virtual void h_answer_read(uint8_t addr, uint32_t data);
    virtual void h_answer_write(uint8_t addr);
    virtual void update();
    virtual void on_mode_entry();
    virtual void on_mode_exit();
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
    HS2VT2_ModeDriver *drivers[64];
    HS2VT2_Interface *interface;
    uint8_t mode_counter;
    uint8_t current_mode;
};

