from random import randrange

class framer_ieee802154:
  #Bits para la palabra de FCF (frame control field)
  #-------------------------------------------------

  #Nota: Los desplazamientos son con respecto a la posicion del octeto, no de la palabra.

  #Tipo de trama
  __frame_type_beacon      = 0b000  #Trama tipo beacon
  __frame_type_data        = 0b001  #Trama de datos
  __frame_type_acknowledge = 0b010  #Trama de acuse de recibo
  __frame_type_mac_command = 0b011  #Trama de comando de MAC

  #Habilitacion del campo de seguridad
  __security_enabled = 0b1 << 3

  #Bandera de trama pendiente
  __frame_pending = 0b1 << 4

  #Bandera de solicitud de acuse de recibo
  __ack_request = 0b1 << 5

  #Bandera de compresion para la ID de PAN
  __pan_id_compression = 0b1 << 6

  #Modo de direccionamiento de destino
  __dst_addr_mode_none       = 0b00 << 2
  __dst_addr_mode_short_addr = 0b10 << 2
  __dst_addr_mode_long_addr  = 0b11 << 2

  #Version de la trama
  __frame_version_2003      = 0b00 << 4
  __frame_version_2006_2011 = 0b01 << 4

  #Modo de direccionamiento de origen
  __src_addr_mode_none       = 0b00 << 6
  __src_addr_mode_short_addr = 0b10 << 6
  __src_addr_mode_long_addr  = 0b11 << 6

  def __init__(self):
    self.__secuencia = randrange(0, 256)

  def escr_pan_id(self, pan_id):
    self.__pan_id = pan_id

  def escr_dir_propia(self, dir_corta):
    self.__dir_propia = dir_corta

  def escr_dir_remota(self, dir_corta):
    self.__dir_remota = dir_corta

  def crear_mpdu(self, payload):
    #Arranca el paquete como una lista vacia
    paquete = []

    #Anexa el FCF
    paquete.append(self.__frame_type_data | self.__ack_request | self.__pan_id_compression);
    paquete.append(self.__dst_addr_mode_short_addr | self.__frame_version_2006_2011 |\
                   self.__src_addr_mode_short_addr)

    #Anexa el numero de secuencia y la incrementa
    paquete.append(self.__secuencia)
    self.__secuencia += 1
    self.__secuencia &= 0xFF

    #Anexa el ID de la PAN de destino
    paquete.append(self.__pan_id & 0xFF)
    paquete.append((self.__pan_id >> 8) & 0xFF)

    #Anexa la direccion de destino
    paquete.append(self.__dir_remota & 0xFF)
    paquete.append((self.__dir_remota >> 8) & 0xFF)

    #Anexa la direccion de origen
    paquete.append(self.__dir_propia & 0xFF)
    paquete.append((self.__dir_propia >> 8) & 0xFF)

    #Anexa el payload de la trama
    paquete.extend(payload)

    #Anexa padding para el FCS (el radio lo calcula automaticamente)
    paquete.extend([0x00] * 2)

    return paquete
