import RPi.GPIO as gpio
from phy.at86rf233_rpi.driver import driver_at86rf233
from mac.ieee802154.driver import driver_ieee802154
from i32ctt.driver import driver_i32ctt

phy = driver_at86rf233(gpio)
mac = driver_ieee802154(phy)
i32ctt = driver_i32ctt(mac)

mac.escr_config_red(canal=26, pan_id=0xCAFE, dir_corta=0x0200)

try:
  registros = i32ctt.escr_registro(0x0100, 0, ((2, 0x0),(3, 0x00FFFFFF)))
  if registros:
    for i in registros:
      print("Registro escrito: {}".format(i))
  else:
    print("No hubo respuesta al escribir")
    exit()

  pares = i32ctt.leer_registro(0x0100, 0, (2,))
  if pares:
    for i in pares:
      print("Registro {}: 0x{:08X}".format(i[0], i[1]))
  else:
    print("No hubo respuesta al leer")

finally:
  gpio.cleanup()
