class HS2VT2_NullInterface: public HS2VT2_Interface {
  public:
    void init();
    void update();
    uint8_t available();
    void send();
};
