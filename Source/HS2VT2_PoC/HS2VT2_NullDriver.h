#ifndef HS2VT2_NullDriver_H
#define HS2VT2_NullDriver_H

class HS2VT2_NullDriver: public HS2VT2_ModeDriver {
  public:
    HS2VT2_NullDriver(uint32_t mode_id);
    uint32_t read(uint8_t addr);
    uint8_t write(uint8_t addr, uint32_t data);
    void h_answer_read(uint8_t addr, uint32_t data);
    void h_answer_write(uint8_t addr);
    void update();
    void on_mode_entry();
    void on_mode_exit();
};

#endif
