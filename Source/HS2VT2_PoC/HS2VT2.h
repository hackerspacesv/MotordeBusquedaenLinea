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

