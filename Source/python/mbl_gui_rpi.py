import RPi.GPIO as gpio
from phy.at86rf233_rpi.driver import driver_at86rf233
from mac.ieee802154.driver import driver_ieee802154
from i32ctt.driver import driver_i32ctt

import time

phy = driver_at86rf233(gpio)
mac = driver_ieee802154(phy)
i32ctt = driver_i32ctt(mac)

def transaccion_prueba(paquete):
  mac.enviar_paquete(paquete)
  print("enviando:")
  for i in paquete:
    print("{:02X}".format(i))

  t_ini = time.clock()
  while True:
    if mac.hay_paquete():
      print("recibido:")
      paquete = mac.recibir_paquete()
      for i in paquete:
        print("{:02X}".format(i))
    if time.clock() - t_ini >= 5.0:
      break

mac.escr_config_red(canal=26, pan_id=0xCAFE, dir_corta=0x0200)
mac.elegir_destino(0x0100)
#print(mac.enviar_paquete([0x01, 0x01, 0x01, 0x00]))
#print(mac.enviar_paquete([0x03, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00]))

try:
  transaccion_prueba([0x03, 0x01, 0x02, 0x00, 4, 3, 2, 1])
  transaccion_prueba([0x01, 0x01, 0x02, 0x00])
finally:
  gpio.cleanup()
