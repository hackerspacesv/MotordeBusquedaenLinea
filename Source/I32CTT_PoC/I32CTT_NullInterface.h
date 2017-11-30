#ifndef I32CTT_NullInterface_H
#define I32CTT_NullInterface_H

class I32CTT_NullInterface: public I32CTT_Interface {
  public:
    void init();
    void update();
    uint8_t available();
    void send();
};

#endif
