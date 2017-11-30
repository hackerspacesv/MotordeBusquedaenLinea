#ifndef I32CTT_ArduinoStreamInterface_H
#define I32CTT_ArduinoStreamInterface_H

#ifdef ARDUINO
#define SER_BUFF_SIZE 255

class I32CTT_ArduinoStreamInterface: public I32CTT_Interface {
  public:
    I32CTT_ArduinoStreamInterface(Stream &port);
    void init();
    void update();
    uint8_t available();
    void send();
  private:
    Stream *port;
    void process_buffer();
    uint8_t data_available = 0;
    uint8_t serial_buffer[SER_BUFF_SIZE];
    uint16_t serial_size = 0;
};

#endif

#endif
