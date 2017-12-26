from kivy.uix.floatlayout import FloatLayout
from kivy.uix.button import Button
from kivy.properties import BooleanProperty

class LayoutEndpoint(FloatLayout):
  habilitado = BooleanProperty(None)
  seleccionado = BooleanProperty(None)
  reg_habilitado = 1

  def __init__(self, i32ctt, direccion_mac, endpoint, **kwargs):
    super(LayoutEndpoint, self).__init__(**kwargs)

    #Se copia la instancia del driver de protocolo y la direccion MAC
    self.i32ctt = i32ctt
    self.direccion_mac = direccion_mac
    self.endpoint = endpoint

    #Se crea, asocia y agrega el boton que controla la habilitacion del endpoint
    self.boton_enable = Button(size_hint = (None, 0.1),\
                               pos_hint = {'right': 1.0, 'top': 0.1},\
                               background_normal = '')
    self.boton_enable.bind(on_press = self.alternar_habilitacion)
    self.add_widget(self.boton_enable)

    #Se asocian la propiedades de habilitacion y seleccion para que se llamen las funciones cada vez
    #que se cambien
    self.bind(habilitado = self.__actualizar_habilitacion)
    self.bind(seleccionado = self.__actualizar_seleccion)

  def alternar_habilitacion(self, instancia):
    #Esta funcion alterna el estado de habilitacion del endpoint
    self.habilitado = not self.habilitado

  def __actualizar_habilitacion(self, *args):
    #Primero se actualiza el estado del boton
    if self.habilitado:
      self.boton_enable.text = 'Encendido'
      self.boton_enable.background_color = (0, 0.5, 0, 1)
    else:
      self.boton_enable.text = 'Apagado'
      self.boton_enable.background_color = (0.75, 0, 0, 1)

    #Luego se traslada el estado del boton al robot
    registros = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,\
                                          ((self.reg_habilitado, self.habilitado),))

    #Si la comunicacion no tuvo exito, fuerza el estado del boton a falso
    if not registros:
      self.habilitado = False

  def __actualizar_seleccion(self, *args):
    #Al seleccionar el endpoint en la GUI, se lee el estado de habilitacion de la misma
    if self.seleccionado:
      registros = self.i32ctt.leer_registro(self.direccion_mac, self.endpoint,\
                                            (self.reg_habilitado,))
      if registros and registros[0][0] == self.reg_habilitado:
        #Si se leyo el registro correctamente, se toma el valor del mismo
        self.habilitado = registros[0][1]
      else:
        self.habilitado = False

  def actualizacion_periodica(self):
    pass
