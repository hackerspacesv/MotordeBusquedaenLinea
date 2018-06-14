from kivy.uix.widget import Widget
from kivy.properties import NumericProperty, BooleanProperty
from kivy.graphics import Color, Line, Rectangle

from mbl_layout_endpoint import LayoutEndpoint

class LayoutEndpointManual(LayoutEndpoint):
  reg_x = 2
  reg_y = 3

  def __init__(self, i32ctt, direccion_mac, endpoint, **kwargs):
    super(LayoutEndpointManual, self).__init__(i32ctt, direccion_mac, endpoint, **kwargs)

    #Se crean los dos controles analogos
    self.control_vertical = ControlAnalogo(orientacion = 'vertical',\
                                           color_fondo = (0, 0, 1),\
                                           color_cursor = (1, 0, 0),\
                                           size_hint=(.1, .8),\
                                           pos_hint={'center_x': .2, 'center_y': .5})
    self.control_horizontal = ControlAnalogo(orientacion = 'horizontal',\
                                             color_fondo = (0, 1, 0),\
                                             color_cursor = (1, 0, 0),\
                                             size_hint=(.6, .15),\
                                             pos_hint={'center_x': .6, 'center_y': .5})

    #Se agregan los controles analogos a este layout
    self.add_widget(self.control_vertical)
    self.add_widget(self.control_horizontal)

  def actualizacion_periodica(self):
    if self.seleccionado:
      #La funcion de actualizacion periodica envia los valores de los controles analogos
      x = int(self.control_horizontal.posicion_cursor * (1 << 24))
      y = int(self.control_vertical.posicion_cursor * (1 << 24))
      registros = self.i32ctt.escr_registros(self.direccion_mac, self.endpoint,\
                                             ((self.reg_x, x), (self.reg_y, y)))

class ControlAnalogo(Widget):
  posicion_cursor = NumericProperty(0)

  def __init__(self, orientacion, color_fondo, color_cursor, **kwargs):
    #Inicializa la clase base
    super(ControlAnalogo, self).__init__(**kwargs)

    #Determina la orientacion solicictada
    if orientacion == 'vertical':
      self.vertical = True
    elif orientacion == 'horizontal':
      self.vertical = False
    else:
      raise ValueError('La orientacion debe ser \'vertical\' u \'horizontal\'')

    #Inicializa las variables de instancia
    self.tocado = False

    #Establece las primitivas de dibujo para este widget
    with self.canvas:
      #color_fondo()
      Color(*color_fondo)
      self.borde = Rectangle(rectangle = self.pos, size = self.size)
      Color(*color_cursor)
      self.cursor = Line(points=self.puntos_cursor(), width=5)

    #Asocia las variables de posicion de widget, dimensiones y posicion de cursor a una funcion
    self.bind(pos=self.actualizar, size = self.actualizar, posicion_cursor = self.actualizar)

  def actualizar(self, *args):
    #Al cambiar las variables, se actualizan las primitivas de dibujo
    self.borde.pos = self.pos
    self.borde.size = self.size
    self.cursor.points = self.puntos_cursor()

  def puntos_cursor(self):
    #Esta funcion devuelve una tupla con las coordenadas X1, Y1, X2 y Y2 de la linea que representa
    #el cursor
    if self.vertical:
      return (self.pos[0],\
              self.pos[1] + self.size[1] * (self.posicion_cursor + 1) / 2,\
              self.pos[0] + self.size[0],\
              self.pos[1] + self.size[1] * (self.posicion_cursor + 1) / 2)
    else:
      return (self.pos[0] + self.size[0] * (self.posicion_cursor + 1) / 2,\
              self.pos[1],\
              self.pos[0] + self.size[0] * (self.posicion_cursor + 1) / 2,\
              self.pos[1] + self.size[1])

  def calcular_cursor(self, touch):
    #Esta funcion devuelve la posicion del cursor segun las coordinadas tactiles pasadas
    if self.vertical:
      if touch.pos[1] < self.pos[1]:
        return -1.0   #El cursor esta por debajo del limite inferior
      elif touch.pos[1] > self.pos[1] + self.size[1]:
        return 1.0    #El cursor esta por arriba del limite superior
      else:
        return (touch.pos[1] - self.pos[1]) / self.size[1] * 2 - 1
    else:
      if touch.pos[0] < self.pos[0]:
        return -1.0   #El cursor esta a la izquierda del limite inferior
      elif touch.pos[0] > self.pos[0] + self.size[0]:
        return 1.0    #El cursor esta a la derecha del limite superior
      else:
        return (touch.pos[0] - self.pos[0]) / self.size[0] * 2 - 1

  def on_touch_down(self, touch):
    #Cuando se hace un nuevo toque sobre la pantalla, si colisiona con este control y no esta siendo
    #tocado previamente, entonces se agarra el toque, se marca el control como tocado y se actualiza
    #la coordenada
    if self.collide_point(*touch.pos) and not self.tocado:
      touch.grab(self)
      self.tocado = True
      self.posicion_cursor = self.calcular_cursor(touch)
      return True

  def on_touch_move(self, touch):
    #Cuando se mueve un toque sobre la pantalla, si esta agarrado por este control, se actualiza la
    #coordenada
    if touch.grab_current is self:
      self.posicion_cursor = self.calcular_cursor(touch)
      return True

  def on_touch_up(self, touch):
    #Cuando se levanta un toque, si esta siedo agarrado por este control, entonces se "desagarra" el
    #toque, se desmarca el control como tocado y se restablece la coordenada al centro
    if touch.grab_current is self:
      touch.ungrab(self)
      self.tocado = False
      self.posicion_cursor = 0
      return True
