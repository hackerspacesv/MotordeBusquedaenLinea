#Framer para el protocolo I32CTT

class framer_i32ctt:
  __read      = 0x01
  __read_ans  = 0x02
  __write     = 0x03
  __write_ans = 0x04
  __list      = 0x05
  __list_ans  = 0x06
  __find      = 0x07
  __find_ans  = 0x08

  def crear_paquete_read(self, endpoint, dir_registros):
    paquete = [self.__read, endpoint & 0xFF]

    for i in dir_registros:
      paquete.append(i & 0xFF)
      paquete.append((i >> 8) & 0xFF)

    return paquete

  def crear_paquete_write(self, endpoint, par_registros):
    #Inicia el paquete con la cabecera, que contiene el comando y el endpoint
    paquete = [self.__write, endpoint & 0xFF]

    for i in par_registros:
      #Verifica que cada pareja sea una tupla o lista con exactamente 2 elementos
      if len(i) != 2:
        raise ValueError('Tupla/lista mal formada')

      #Anexa la pareja al paquete, la direccion es de 16 bits y los datos de 32
      paquete.extend(self.__descomponer_u16(i[0]))
      paquete.extend(self.__descomponer_u32(i[1]))

    return paquete

  def leer_paquete_read_ans(self, paquete):
    if paquete[0] != self.__read_ans:
      return ()

    endpoint = paquete[1]

    paquete = paquete[2:]
    if len(paquete) % 6 != 0:
      return ()

    datos = []
    p = 0
    while p < len(paquete):
      direccion = self.__ensamblar_u16(paquete[p: p + 2])
      p += 2
      dato = self.__ensamblar_u32(paquete[p: p + 4])
      p += 4
      datos.append((direccion, dato))

    return datos

  def leer_paquete_write_ans(self, paquete):
    if paquete[0] != self.__write_ans:
      return ()

    endpoint = paquete[1]

    paquete = paquete[2:]
    if len(paquete) % 2 != 0:
      return ()

    datos = []
    p = 0
    while p < len(paquete):
      direccion = self.__ensamblar_u16(paquete[p: p + 2])
      p += 2
      datos.append(direccion)

    return datos

  def __ensamblar_u16(self, octetos):
    return octetos[0] | (octetos[1] << 8)

  def __ensamblar_u32(self, octetos):
    return octetos[0] | (octetos[1] << 8) | (octetos[2] << 16)  | (octetos[3] << 24)

  def __descomponer_u16(self, entero):
    return (entero & 0xFF, (entero >> 8) & 0xFF)

  def __descomponer_u32(self, entero):
    return (entero & 0xFF, (entero >> 8) & 0xFF, (entero >> 16) & 0xFF, (entero >> 24) & 0xFF)
