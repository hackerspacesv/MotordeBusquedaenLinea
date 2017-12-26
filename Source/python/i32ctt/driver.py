#Framer para el protocolo I32CTT

from framer import framer_i32ctt
import time

class driver_i32ctt:
  __timeout = 0.1

  def __init__(self, mac):
    self.__mac = mac
    self.__framer = framer_i32ctt()

  def leer_registro(self, destino, endpoint, dir_registros):
    #Se forma el paquete de I32CTT mediante el framer
    paquete = self.__framer.crear_paquete_read(endpoint, dir_registros)

    #Envia el paquete a la capa subyacente
    self.__mac.enviar_paquete(destino, paquete)

    #Recibe la respuesta y la retorna
    return self.__recibir_respuesta(self.__framer.leer_paquete_read_ans)

  def escr_registro(self, destino, endpoint, par_registros):
    #Se forma el paquete de I32CTT mediante el framer
    paquete = self.__framer.crear_paquete_write(endpoint, par_registros)

    #Envia el paquete a la capa subyacente
    self.__mac.enviar_paquete(destino, paquete)

    #Recibe la respuesta y la retorna
    return self.__recibir_respuesta(self.__framer.leer_paquete_write_ans)

  def __recibir_respuesta(self, func):
    #Inicia el proceso de espera de respuesta, tomando el tiempo inicial
    t_ini = time.time()
    while True:
      if self.__mac.hay_paquete():
        origen, paquete = self.__mac.recibir_paquete()
        #print ("respuesta de {:02X}".format(origen))
        #print("recibido:")
        #for i in paquete:
          #print("{:02X}".format(i))
        
        #TODO: Verificar el origen antes de descodificar el paquete y retornarlo
        return func(paquete)

      if time.time() - t_ini < self.__timeout:
        time.sleep(0.001)
      else:
        return ()
