from kivy.app import App
from kivy.clock import Clock

from mbl_kivy.mbl_ventana_principal import VentanaPrincipal

#gpio = None
#from phy.nulo.driver import driver_phy_nulo as driver_phy
#from mac.nulo.driver import driver_mac_nulo as driver_mac
import RPi.GPIO as gpio
from phy.at86rf233_rpi.driver import driver_at86rf233 as driver_phy
from mac.ieee802154.driver import driver_ieee802154 as driver_mac
from i32ctt.driver import driver_i32ctt

direccion_mbl = 0x0100
ep_manu = 0
ep_manu_x = 2
ep_manu_y = 3

#Clase para la aplicacion
class MyApp(App):
  def build(self):
    self.ventana = VentanaPrincipal(i32ctt, direccion_mbl, (0, 1, 2))
    return self.ventana

#Funcion de actualizacion periodica
def actualizacion_periodica(dt):
  app.ventana.actualizacion_periodica()

if __name__ == '__main__':
  phy = driver_phy(gpio)
  mac = driver_mac(phy)
  i32ctt = driver_i32ctt(mac)

  mac.escr_config_red(canal=26, pan_id=0xCAFE, dir_corta=0x0200)

  Clock.schedule_interval(actualizacion_periodica, 0.01)

  try:
    app = MyApp()
    app.run()
  finally:
    if gpio and gpio.cleanup:
      gpio.cleanup()
