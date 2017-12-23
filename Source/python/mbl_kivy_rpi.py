from kivy.app import App
from kivy.properties import NumericProperty
from kivy.uix.widget import Widget
from kivy.uix.floatlayout import FloatLayout
from kivy.graphics import Color, Line, Rectangle

import RPi.GPIO as gpio
from phy.at86rf233_rpi.driver import driver_at86rf233
from mac.ieee802154.driver import driver_ieee802154
from i32ctt.driver import driver_i32ctt

class control_vertical(Widget):
  posicion_cursor = NumericProperty(0)

  def __init__(self, **kwargs):
    super(control_vertical, self).__init__(**kwargs)

    with self.canvas:
      Color(0, 0, 1, mode='rgb')
      self.borde = Rectangle(rectangle = self.pos, size = self.size)
      Color(1, 0, 0, mode='rgb')
      self.cursor = Line(points=self.puntos_cursor(), width=5)

    self.bind(pos=self.actualizar, size = self.actualizar, posicion_cursor = self.actualizar)

  def actualizar(self, *args):
    self.borde.pos = self.pos
    self.borde.size = self.size
    self.cursor.points = self.puntos_cursor()
    y = int(self.posicion_cursor*(1<<24))
    registros = i32ctt.escr_registro(0x0100, 1, ((3, y),))

  def puntos_cursor(self):
    return self.pos[0],                self.pos[1] + self.size[1] * (self.posicion_cursor + 1) / 2,\
           self.pos[0] + self.size[0], self.pos[1] + self.size[1] * (self.posicion_cursor + 1) / 2

  def valor_cursor(self, touch):
    return (touch.pos[1] - self.pos[1]) / self.size[1] * 2 - 1

  def on_touch_down(self, touch):
    if self.collide_point(touch.pos[0], touch.pos[1]):
      self.posicion_cursor = self.valor_cursor(touch)
      return True
    else:
      return False

  def on_touch_move(self, touch):
    if self.collide_point(touch.pos[0], touch.pos[1]):
      self.posicion_cursor = self.valor_cursor(touch)
      return True
    else:
      return False

  def on_touch_up(self, touch):
    if self.collide_point(touch.pos[0], touch.pos[1]):
      self.posicion_cursor = 0
      return True
    else:
      return False

class control_horizontal(Widget):
  posicion_cursor = NumericProperty(0)

  def __init__(self, **kwargs):
    super(control_horizontal, self).__init__(**kwargs)

    with self.canvas:
      Color(0, 1, 0, mode='rgb')
      self.borde = Rectangle(rectangle = self.pos, size = self.size)
      Color(1, 0, 0, mode='rgb')
      self.cursor = Line(points=self.puntos_cursor(), width=5)

    self.bind(pos=self.actualizar, size = self.actualizar, posicion_cursor = self.actualizar)

  def actualizar(self, *args):
    self.borde.pos = self.pos
    self.borde.size = self.size
    self.cursor.points = self.puntos_cursor()
    x = int(self.posicion_cursor*(1<<24))
    registros = i32ctt.escr_registro(0x0100, 1, ((2, x),))

  def puntos_cursor(self):
    return self.pos[0] + self.size[0] * (self.posicion_cursor + 1) / 2, self.pos[1],\
           self.pos[0] + self.size[0] * (self.posicion_cursor + 1) / 2, self.pos[1] + self.size[1]

  def valor_cursor(self, touch):
    return (touch.pos[0] - self.pos[0]) / self.size[0] * 2 - 1

  def on_touch_down(self, touch):
    if self.collide_point(touch.pos[0], touch.pos[1]):
      self.posicion_cursor = self.valor_cursor(touch)
      return True
    else:
      return False

  def on_touch_move(self, touch):
    if self.collide_point(touch.pos[0], touch.pos[1]):
      self.posicion_cursor = self.valor_cursor(touch)
      return True
    else:
      return False

  def on_touch_up(self, touch):
    if self.collide_point(touch.pos[0], touch.pos[1]):
      self.posicion_cursor = 0
      return True
    else:
      return False

class ventana_principal(FloatLayout):
  def __init__(self, **kwargs):
    super(ventana_principal, self).__init__(**kwargs)
    self.add_widget(control_vertical(size_hint=(.1, .8), pos_hint={'center_x': .2, 'center_y': .5}))
    self.add_widget(control_horizontal(size_hint=(.6, .15), pos_hint={'center_x': .6, 'center_y': .5}))

class MyApp(App):
  def build(self):
    ventana = ventana_principal()
    return ventana

if __name__ == '__main__':
  phy = driver_at86rf233(gpio)
  mac = driver_ieee802154(phy)
  i32ctt = driver_i32ctt(mac)

  mac.escr_config_red(canal=26, pan_id=0xCAFE, dir_corta=0x0200)

  try:
    MyApp().run()
  finally:
    gpio.cleanup()
