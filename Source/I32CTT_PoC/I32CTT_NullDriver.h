#ifndef I32CTT_NullDriver_H
#define I32CTT_NullDriver_H

class I32CTT_NullDriver: public I32CTT_ModeDriver {
  public:
    I32CTT_NullDriver(uint32_t mode_id);
    uint32_t read(uint8_t addr);
    uint8_t write(uint8_t addr, uint32_t data);
    void h_answer_read(uint8_t addr, uint32_t data);
    void h_answer_write(uint8_t addr);
    void update();
    void on_mode_entry();
    void on_mode_exit();
};

#endif
