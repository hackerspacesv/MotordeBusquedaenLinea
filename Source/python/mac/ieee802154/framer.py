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
  __frame_type_mask        = 0b111  #Mascara del campo

  #Habilitacion del campo de seguridad
  __security_enabled      = 0b1 << 3
  __security_enabled_mask = 0b1 << 3

  #Bandera de trama pendiente
  __frame_pending      = 0b1 << 4
  __frame_pending_mask = 0b1 << 4

  #Bandera de solicitud de acuse de recibo
  __ack_request      = 0b1 << 5
  __ack_request_mask = 0b1 << 5

  #Bandera de compresion para la ID de PAN
  __pan_id_compression      = 0b1 << 6
  __pan_id_compression_mask = 0b1 << 6

  #Modo de direccionamiento de destino
  __dst_addr_mode_none       = 0b00 << 2
  __dst_addr_mode_short_addr = 0b10 << 2
  __dst_addr_mode_long_addr  = 0b11 << 2
  __dst_addr_mode_mask       = 0b11 << 2

  #Version de la trama
  __frame_version_2003      = 0b00 << 4
  __frame_version_2006_2011 = 0b01 << 4
  __frame_version_mask      = 0b11 << 4

  #Modo de direccionamiento de origen
  __src_addr_mode_none       = 0b00 << 6
  __src_addr_mode_short_addr = 0b10 << 6
  __src_addr_mode_long_addr  = 0b11 << 6
  __src_addr_mode_mask       = 0b11 << 6

  def __init__(self):
    self.__secuencia = randrange(0, 256)

  def escr_pan_id(self, pan_id):
    self.__pan_id = pan_id

  def escr_dir_corta(self, dir_corta):
    self.__dir_corta = dir_corta

  def crear_mpdu(self, destino, payload):
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
    paquete.append(destino & 0xFF)
    paquete.append((destino >> 8) & 0xFF)

    #Anexa la direccion de origen
    paquete.append(self.__dir_corta & 0xFF)
    paquete.append((self.__dir_corta >> 8) & 0xFF)

    #Anexa el payload de la trama
    paquete.extend(payload)

    #Anexa padding para el FCS (el radio lo calcula automaticamente)
    paquete.extend([0x00] * 2)

    return paquete

  def extraer_payload(self, paquete):
    #Se filtran paquetes de datos que tengan la seguridad apagada y la compresion de pan encendida
    filtro = self.__frame_type_mask | self.__security_enabled_mask | self.__pan_id_compression_mask 
    patron = self.__frame_type_data | self.__pan_id_compression 

    if paquete[0] & filtro != patron:
      return ()

    #Se filtran solo paquetes con direcciones cortas
    filtro = self.__dst_addr_mode_mask | self.__src_addr_mode_mask
    patron = self.__dst_addr_mode_short_addr | self.__src_addr_mode_short_addr
    if paquete[1] & filtro != patron:
      return ()

    #Se extrae la direccion de origen del paquete y su payload
    dir_corta = paquete[7] | (paquete[8] << 8)
    payload = paquete[9:len(paquete) - 2]

    return dir_corta, payload
