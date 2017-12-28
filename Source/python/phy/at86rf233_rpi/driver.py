import spidev
import time

class driver_at86rf233:
  #Estados de interes del radio, usados en la funcion de cambio de estado
  ESTADO_RX_AACK = 0
  ESTADO_TX_ARET = 1

  #Clase vacia usada como contenedor de constantes
  class clase_vacia:
    pass

  #Pines del radio (segun numeracion de tablero)
  __pin = clase_vacia()
  __pin.IRQ    = 16
  __pin.RST    = 18
  __pin.SLP_TR = 22

  #Comandos de bus SPI para el radio
  __cmd_spi = clase_vacia()
  __cmd_spi.REG_READ   = 0x80
  __cmd_spi.REG_WRITE  = 0xC0
  __cmd_spi.FB_READ    = 0x20
  __cmd_spi.FB_WRITE   = 0x60
  __cmd_spi.SRAM_READ  = 0x00
  __cmd_spi.SRAM_WRITE = 0x40

  #Registro TRX_STATUS
  __TRX_STATUS                                         = clase_vacia()
  __TRX_STATUS.addr                                    = 0x01
  __TRX_STATUS.TRX_STATUS                              = clase_vacia()
  __TRX_STATUS.TRX_STATUS.mask                         = 0x1F
  __TRX_STATUS.TRX_STATUS.P_ON                         = 0x00
  __TRX_STATUS.TRX_STATUS.BUSY_RX                      = 0x01
  __TRX_STATUS.TRX_STATUS.BUSY_TX                      = 0x02
  __TRX_STATUS.TRX_STATUS.RX_ON                        = 0x06
  __TRX_STATUS.TRX_STATUS.TRX_OFF                      = 0x08
  __TRX_STATUS.TRX_STATUS.PLL_ON                       = 0x09
  __TRX_STATUS.TRX_STATUS.SLEEP                        = 0x0F
  __TRX_STATUS.TRX_STATUS.PREP_DEEP_SLEEP              = 0x10
  __TRX_STATUS.TRX_STATUS.BUSY_RX_AACK                 = 0x11
  __TRX_STATUS.TRX_STATUS.BUSY_TX_ARET                 = 0x12
  __TRX_STATUS.TRX_STATUS.RX_AACK_ON                   = 0x16
  __TRX_STATUS.TRX_STATUS.TX_ARET_ON                   = 0x19
  __TRX_STATUS.TRX_STATUS.STATE_TRANSITION_IN_PROGRESS = 0x1F

  #Registro TRX_STATE
  __TRX_STATE                                    = clase_vacia()
  __TRX_STATE.addr                               = 0x02
  __TRX_STATE.TRX_CMD                            = clase_vacia()
  __TRX_STATE.TRX_CMD.mask                       = 0x1F
  __TRX_STATE.TRX_CMD.NOP                        = 0x00
  __TRX_STATE.TRX_CMD.TX_START                   = 0x02
  __TRX_STATE.TRX_CMD.FORCE_TRX_OFF              = 0x03
  __TRX_STATE.TRX_CMD.FORCE_PLL_ON               = 0x04
  __TRX_STATE.TRX_CMD.RX_ON                      = 0x06
  __TRX_STATE.TRX_CMD.TRX_OFF                    = 0x08
  __TRX_STATE.TRX_CMD.PLL_ON                     = 0x09
  __TRX_STATE.TRX_CMD.PREP_DEEP_SLEEP            = 0x10
  __TRX_STATE.TRX_CMD.RX_AACK_ON                 = 0x16
  __TRX_STATE.TRX_CMD.TX_ARET_ON                 = 0x19
  __TRX_STATE.TRAC_STATUS                        = clase_vacia()
  __TRX_STATE.TRAC_STATUS.mask                   = 0xE0
  __TRX_STATE.TRAC_STATUS.SUCCESS                = 0x00 
  __TRX_STATE.TRAC_STATUS.SUCCESS_DATA_PENDING   = 0x20
  __TRX_STATE.TRAC_STATUS.SUCCESS_WAIT_FOR_ACK   = 0x40
  __TRX_STATE.TRAC_STATUS.CHANNEL_ACCESS_FAILURE = 0x60
  __TRX_STATE.TRAC_STATUS.NO_ACK                 = 0xA0
  __TRX_STATE.TRAC_STATUS.INVALID                = 0xE0

  #Registro PHY_CC_CCA
  __PHY_CC_CCA                  = clase_vacia()
  __PHY_CC_CCA.addr             = 0x08
  __PHY_CC_CCA.CHANNEL          = clase_vacia()
  __PHY_CC_CCA.CHANNEL.mask     = 0x1F
  __PHY_CC_CCA.CCA_MODE         = clase_vacia()
  __PHY_CC_CCA.CCA_MODE.mask    = 0x60
  __PHY_CC_CCA.CCA_MODE.Mode_3a = 0x00
  __PHY_CC_CCA.CCA_MODE.Mode_1  = 0x20
  __PHY_CC_CCA.CCA_MODE.Mode_2  = 0x40
  __PHY_CC_CCA.CCA_MODE.Mode_3b = 0x60
  __PHY_CC_CCA.CCA_REQUEST      = clase_vacia()
  __PHY_CC_CCA.CCA_REQUEST.mask = 0x80

  #Registro TRX_CTRL_2
  __TRX_CTRL_2                      = clase_vacia()
  __TRX_CTRL_2.addr                 = 0x0C
  __TRX_CTRL_2.RX_SAFE_MODE         = clase_vacia()
  __TRX_CTRL_2.RX_SAFE_MODE.mask    = 0x80
  __TRX_CTRL_2.OQPSK_SCRAM_EN       = clase_vacia()
  __TRX_CTRL_2.OQPSK_SCRAM_EN.mask  = 0x20
  __TRX_CTRL_2.OQPSK_DATA_RATE      = clase_vacia()
  __TRX_CTRL_2.OQPSK_DATA_RATE.mask = 0x07

  #Registro IRQ_MASK
  __IRQ_MASK                        = clase_vacia()
  __IRQ_MASK.addr                   = 0x0E
  __IRQ_MASK.IRQ_0_PLL_LOCK         = clase_vacia()
  __IRQ_MASK.IRQ_0_PLL_LOCK.mask    = 0x01
  __IRQ_MASK.IRQ_1_PLL_UNLOCK       = clase_vacia()
  __IRQ_MASK.IRQ_1_PLL_UNLOCK.mask  = 0x02
  __IRQ_MASK.IRQ_2_RX_START         = clase_vacia()
  __IRQ_MASK.IRQ_2_RX_START.mask    = 0x04
  __IRQ_MASK.IRQ_3_TRX_END          = clase_vacia()
  __IRQ_MASK.IRQ_3_TRX_END.mask     = 0x08
  __IRQ_MASK.IRQ_4_CCA_ED_DONE      = clase_vacia()
  __IRQ_MASK.IRQ_4_CCA_ED_DONE.mask = 0x10
  __IRQ_MASK.IRQ_5_AMI              = clase_vacia()
  __IRQ_MASK.IRQ_5_AMI.mask         = 0x20
  __IRQ_MASK.IRQ_6_TRX_UR           = clase_vacia()
  __IRQ_MASK.IRQ_6_TRX_UR.mask      = 0x40
  __IRQ_MASK.IRQ_7_BAT_LOW          = clase_vacia()
  __IRQ_MASK.IRQ_7_BAT_LOW.mask     = 0x80

  #Registro IRQ_STATUS
  __IRQ_STATUS                        = clase_vacia()
  __IRQ_STATUS.addr                   = 0x0F
  __IRQ_STATUS.IRQ_0_PLL_LOCK         = clase_vacia()
  __IRQ_STATUS.IRQ_0_PLL_LOCK.mask    = 0x01
  __IRQ_STATUS.IRQ_1_PLL_UNLOCK       = clase_vacia()
  __IRQ_STATUS.IRQ_1_PLL_UNLOCK.mask  = 0x02
  __IRQ_STATUS.IRQ_2_RX_START         = clase_vacia()
  __IRQ_STATUS.IRQ_2_RX_START.mask    = 0x04
  __IRQ_STATUS.IRQ_3_TRX_END          = clase_vacia()
  __IRQ_STATUS.IRQ_3_TRX_END.mask     = 0x08
  __IRQ_STATUS.IRQ_4_CCA_ED_DONE      = clase_vacia()
  __IRQ_STATUS.IRQ_4_CCA_ED_DONE.mask = 0x10
  __IRQ_STATUS.IRQ_5_AMI              = clase_vacia()
  __IRQ_STATUS.IRQ_5_AMI.mask         = 0x20
  __IRQ_STATUS.IRQ_6_TRX_UR           = clase_vacia()
  __IRQ_STATUS.IRQ_6_TRX_UR.mask      = 0x40
  __IRQ_STATUS.IRQ_7_BAT_LOW          = clase_vacia()
  __IRQ_STATUS.IRQ_7_BAT_LOW.mask     = 0x80

  #Registros SHORT_ADDR_0 y SHORT_ADDR_1
  __SHORT_ADDR_0      = clase_vacia()
  __SHORT_ADDR_0.addr = 0x20
  __SHORT_ADDR_1      = clase_vacia()
  __SHORT_ADDR_1.addr = 0x21

  #Registros PAN_ID_0 y PAN_ID_1
  __PAN_ID_0      = clase_vacia
  __PAN_ID_0.addr = 0x22
  __PAN_ID_1      = clase_vacia()
  __PAN_ID_1.addr = 0x23

  def __init__(self, gpio):
    #Crea el objeto del bus SPI0, linea CS0 y lo inicializa a 1MHz
    self.__spi = spidev.SpiDev()
    self.__spi.open(0,0)
    self.__spi.max_speed_hz = 1000000

    #Configura los pines de I/O usados para el radio
    self.__gpio = gpio
    self.__gpio.setmode(self.__gpio.BOARD)
    self.__gpio.setup(self.__pin.IRQ, self.__gpio.IN)#, pull_up_down=self.__gpio.PUD_UP);
    self.__gpio.setup(self.__pin.RST, self.__gpio.OUT)
    self.__gpio.setup(self.__pin.SLP_TR, self.__gpio.OUT);

    #Llama a la rutina de inicializacion del radio
    self.__reset()

    #Habilita la proteccion dinamica del frame buffer, para evitar que los paquetes recibidos sean
    #sobre escritos hasta que sean leidos.
    self.__escr_campo_modo_seguro_rx(True)

    #Lee el registro de estado de interrupciones para asegurarse de limpiar el estado de las mismas
    #antes de habilitarlas
    self.__leer_reg(self.__IRQ_STATUS.addr)

    #Habilita la interrupcion del radio de paquetes completados
    self.__escr_reg(self.__IRQ_MASK.addr, self.__IRQ_MASK.IRQ_3_TRX_END.mask)

    #Una vez inicializado, se coloca el radio en modo de recepcion.
    self.__cambiar_estado(self.ESTADO_RX_AACK);

    #Establece el callback de IRQ:
    #gpio.add_event_detect(self.__pin_IRQ, gpio.FALLING, callback=self.__irq_callback)

  #def __irq_callback():
    #return

  def escr_canal(self, canal):
    self.__escr_campo_canal(canal)

  def escr_pan_id(self, pan_id):
    self.__escr_reg(self.__PAN_ID_0.addr, pan_id & 0xFF)
    self.__escr_reg(self.__PAN_ID_1.addr, (pan_id >> 8) & 0xFF)

  def escr_dir_corta(self, dir_corta):
    self.__escr_reg(self.__SHORT_ADDR_0.addr, dir_corta & 0xFF)
    self.__escr_reg(self.__SHORT_ADDR_1.addr, (dir_corta >> 8) & 0xFF)

  def prueba(self):
    #print("bits_per_word = " + str(spi.bits_per_word))
    #print("cshigh = " + str(spi.cshigh))
    #print("lsbfirst = " + str(spi.lsbfirst))
    #print("max_speed_hz = " + str(spi.max_speed_hz))
    #print("mode = " + str(spi.mode))
    #print(self.__leer_buffer())
    pass

  def enviar_paquete(self, paquete):
    #Coloca el radio en modo de transmision
    self.__cambiar_estado(self.ESTADO_TX_ARET)

    #Prepara el paquete en el buffer de salida
    self.__escr_buffer(paquete)

    #Activa la linea SLP_TR para iniciar la transmision del paquete
    self.__gpio.output(self.__pin.SLP_TR, self.__gpio.HIGH);
    self.__gpio.output(self.__pin.SLP_TR, self.__gpio.LOW);

    #Espera a que el radio active la linea de interrupcion por el paquete recien enviado
    t_ini = time.time()
    while True:
      #Determina si el radio activo la linea de interrupcion
      if self.__gpio.input(self.__pin.IRQ) == self.__gpio.HIGH:
        #Verifica que la interrupcion generada sea la de fin de transmision
        if self.__leer_reg(self.__IRQ_STATUS.addr) & self.__IRQ_STATUS.IRQ_3_TRX_END.mask != 0:
          #Verifica que el resultado de la transaccion sea exitoso
          if self.__leer_reg(self.__TRX_STATE.addr) & self.__TRX_STATE.TRAC_STATUS.mask ==\
              self.__TRX_STATE.TRAC_STATUS.SUCCESS:
            #Si fue exitoso, continua
            break
          else:
            #Si fallo, retorna con error
            return False

      #Determina si ya elapso el tiempo maximo de espera
      if time.time() - t_ini >= 0.05:
        return False

    #Restablece el radio en modo de recepcion
    self.__cambiar_estado(self.ESTADO_RX_AACK)

    #Retorna exitosamente
    return True

  def hay_paquete(self):
    #Determina si la linea de interrupcion esta activa
    if self.__gpio.input(self.__pin.IRQ) == self.__gpio.LOW:
      return False

    #Determina si la bandera de interrupcion de paquete recibido esta activa
    if self.__leer_reg(self.__IRQ_STATUS.addr) & self.__IRQ_STATUS.IRQ_3_TRX_END.mask == 0:
      return False

    #Determina si la ultima transaccion fue exitosa
    trac_status = self.__leer_reg(self.__TRX_STATE.addr) & self.__TRX_STATE.TRAC_STATUS.mask
    if trac_status == self.__TRX_STATE.TRAC_STATUS.SUCCESS or \
       trac_status == self.__TRX_STATE.TRAC_STATUS.SUCCESS_WAIT_FOR_ACK:
      return True
    else:
      return False

  def recibir_paquete(self):
    return self.__leer_buffer()

  def __reset(self):
    #Coloca SLP_TR en nivel bajo (inactivo)
    self.__gpio.output(self.__pin.SLP_TR, self.__gpio.LOW);

    #Inicializa el radio para colocarlo en un estado conocido
    self.__gpio.output(self.__pin.RST, self.__gpio.LOW)
    time.sleep(0.1);
    self.__gpio.output(self.__pin.RST, self.__gpio.HIGH)
    time.sleep(0.1);

  def __escr_reg(self, reg, dato):
    self.__spi.xfer2([self.__cmd_spi.REG_WRITE | reg, dato])

  def __leer_reg(self, reg):
    r = self.__spi.xfer2([self.__cmd_spi.REG_READ | reg, 0])
    return r[1]

  def __escr_campo_canal(self, canal):
    #Toma el valor original del registro, aisla el campo CCA_MODE y le sobrepone el nuevo canal
    reg = self.__leer_reg(self.__PHY_CC_CCA.addr) & self.__PHY_CC_CCA.CCA_MODE.mask
    self.__escr_reg(self.__PHY_CC_CCA.addr, reg | canal)

  def __escr_campo_modo_seguro_rx(self, valor):
    #Toma el valor del registro y preserva los campos no relacionados, excepto el que se sobre
    #escribira, que tomara el valor de 0
    reg = self.__leer_reg(self.__TRX_CTRL_2.addr) &\
          (self.__TRX_CTRL_2.OQPSK_SCRAM_EN.mask | self.__TRX_CTRL_2.OQPSK_DATA_RATE.mask)

    #Si el valor pasado es verdadero, sobrepone el bit del campo de modo seguro
    if valor:
      reg |= self.__TRX_CTRL_2.RX_SAFE_MODE.mask

    #Se escribe el nuevo valor del registro
    self.__escr_reg(self.__TRX_CTRL_2.addr, reg)

  def __leer_buffer(self):
    #Lee la longitud del payload del frame buffer usando un acceso desde RAM.
    longitud = self.__spi.xfer2([self.__cmd_spi.SRAM_READ, 0x00, 0x00])[2]

    #Lee el payload usando un acceso al frame buffer (se retiran los 2 primeros bytes de estado de
    #PHY y el PHR).
    return self.__spi.xfer2([self.__cmd_spi.FB_READ] + ([0x00] * (1 + longitud)))[2:]

    #Nota: Las raspberry pi no pueden sostener chip select entre transacciones SPI, por lo que no
    #se puede leer la longitud del payload (PHR) a media transaccion de frame buffer. El acceso a
    #SRAM permite obtener esta longitud sin perder la garantia de la proteccion dinamica del frame
    #buffer, es decir, sin riesgo a que sea sobre escrito por un nuevo paquete.

  def __escr_buffer(self, datos):
    #Envia el paquete completo por SPI, que incluye el comando de escritura al frame buffer, la
    #longitud de la trama y el contenido de la trama
    self.__spi.xfer2([self.__cmd_spi.FB_WRITE, len(datos)] + datos)

  def __cambiar_estado(self, objetivo):
    #El lazo se repite hasta alcanzar el estado deseado
    estado_alcanzado = False
    while (not estado_alcanzado):
      #Lee el estado actual rel radio
      s = self.__leer_reg(self.__TRX_STATUS.addr) & self.__TRX_STATUS.TRX_STATUS.mask

      #Se determina la accion a realizar dependiendo del estado actual
      if s == self.__TRX_STATUS.TRX_STATUS.TRX_OFF:
        #El estado inicial puede transicionar directamente al estado deseado
        if objetivo == self.ESTADO_RX_AACK:
          self.__escr_reg(self.__TRX_STATE.addr, self.__TRX_STATE.TRX_CMD.RX_AACK_ON)
        elif objetivo == self.ESTADO_TX_ARET:
          self.__escr_reg(self.__TRX_STATE.addr, self.__TRX_STATE.TRX_CMD.TX_ARET_ON)
      elif s == self.__TRX_STATUS.TRX_STATUS.BUSY_RX_AACK or\
            s == self.__TRX_STATUS.TRX_STATUS.BUSY_TX_ARET or\
            s == self.__TRX_STATUS.TRX_STATUS.STATE_TRANSITION_IN_PROGRESS:
        #Los estados de ocupado son transicionales y terminan eventualmente
        pass
      elif s == self.__TRX_STATUS.TRX_STATUS.RX_AACK_ON:
        #Estado de recepcion
        if objetivo == self.ESTADO_RX_AACK:
          estado_alcanzado = True
        elif objetivo == self.ESTADO_TX_ARET:
          self.__escr_reg(self.__TRX_STATE.addr, self.__TRX_STATE.TRX_CMD.TX_ARET_ON)
      elif s == self.__TRX_STATUS.TRX_STATUS.TX_ARET_ON:
        #Estado de transmision
        if objetivo == self.ESTADO_RX_AACK:
          self.__escr_reg(self.__TRX_STATE.addr, self.__TRX_STATE.TRX_CMD.RX_AACK_ON)
        elif objetivo == self.ESTADO_TX_ARET:
          estado_alcanzado = True
      elif s == self.__TRX_STATUS.TRX_STATUS.P_ON or\
           s == self.__TRX_STATUS.TRX_STATUS.RX_ON or\
           s == self.__TRX_STATUS.TRX_STATUS.PLL_ON or \
           s == self.__TRX_STATUS.TRX_STATUS.PREP_DEEP_SLEEP:
        #Estos estados no son usados, pero en caso de estar en ellos simplemente se abandonan
        self.__escr_reg(self.__TRX_STATE.addr, self.__TRX_STATE.TRX_CMD.TRX_OFF)
      else:
        #Para cualquier otro estado se intenta recuperar al radio forzandolo al estado TRX_OFF
        #Nota: Esto incluye los estados BUSY_RX y BUSY_TX, ademas de cualquier estado invalido
        self.__escr_reg(self.__TRX_STATE.addr, self.__TRX_STATE.TRX_CMD.FORCE_TRX_OFF)

  def __del__(self):
    self.__spi.close()
