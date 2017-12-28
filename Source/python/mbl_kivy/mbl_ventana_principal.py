from kivy.uix.boxlayout import BoxLayout
from kivy.uix.stacklayout import StackLayout
from kivy.uix.button import Button
from kivy.properties import ObjectProperty

from mbl_endpoint_manual import LayoutEndpointManual
from mbl_endpoint_automatico import LayoutEndpointAutomatico
from mbl_endpoint_sensores import LayoutEndpointSensores

class VentanaPrincipal(BoxLayout):
  layout_actual = ObjectProperty(None)

  def __init__(self, i32ctt, direccion_mac, endpoints, **kwargs):
    super(VentanaPrincipal, self).__init__(orientation = 'vertical', **kwargs)

    #Se crea el layout con los botones superiores y se agrega a la ventana principal
    self.layout_botones = StackLayout(orientation = 'rl-tb', size_hint = (1.0, 0.1))
    self.add_widget(self.layout_botones)

    #Se crean los botones
    self.boton_endpoint_manual = Button(text = 'Manual', size_hint = (None, 1.0))
    self.boton_endpoint_automatico = Button(text = 'Automatico', size_hint = (None, 1.0))
    self.boton_endpoint_sensores = Button(text = 'Sensores', size_hint = (None, 1.0))

    #Se agregan los botones al layout correspondiente
    #TODO: Habilitar una vez este implementado
    #self.layout_botones.add_widget(self.boton_endpoint_sensores)
    self.layout_botones.add_widget(self.boton_endpoint_automatico)
    self.layout_botones.add_widget(self.boton_endpoint_manual)

    #Se asocian los eventos de click de los botones con la funcion correspondiente
    self.boton_endpoint_manual.bind(on_press = self.__boton_presionado)
    self.boton_endpoint_automatico.bind(on_press = self.__boton_presionado)
    self.boton_endpoint_sensores.bind(on_press = self.__boton_presionado)

    #Se crean los layouts con las interfases de los endpoints
    self.layout_endpoint_manual = LayoutEndpointManual(i32ctt, direccion_mac, endpoints[0])
    self.layout_endpoint_automatico = LayoutEndpointAutomatico(i32ctt, direccion_mac, endpoints[1])
    self.layout_endpoint_sensores = LayoutEndpointSensores(i32ctt, direccion_mac, endpoints[2])

    #Se asocia la propiedad de layout actual a la funcion que actualiza sus cambios
    self.bind(layout_actual = self.__actualizar_layout)

    #Se selecciona el layout de control manual por defecto
    self.layout_actual = self.layout_endpoint_manual

  def __actualizar_layout(self, *args):
    #En caso de tener exactamente 2 hijos (el layout de los botones y el actualmente seleccionado),
    #se remueve el layout actualmente seleccionado (que es el ultimo agregado, o el primero de la
    #lista). Si tiene solo 1 hijo, son solo los botones y no deberian removerse.
    if len(self.children) == 2:
      self.children[0].seleccionado = False
      self.remove_widget(self.children[0])

    #Ahora se agrega el layout actualmente seleccionado, y se invoca su evento de seleccion
    self.add_widget(self.layout_actual)
    self.layout_actual.seleccionado = True

  def __boton_presionado(self, boton):
    #Se determina el layout solicitado de acuerdo con el boton presionado
    if boton is self.boton_endpoint_manual:
      self.layout_actual = self.layout_endpoint_manual
    elif boton is self.boton_endpoint_automatico:
      self.layout_actual = self.layout_endpoint_automatico
    elif boton is self.boton_endpoint_sensores:
      self.layout_actual = self.layout_endpoint_sensores

  def actualizacion_periodica(self):
    self.layout_endpoint_manual.actualizacion_periodica()
    self.layout_endpoint_automatico.actualizacion_periodica()
    self.layout_endpoint_sensores.actualizacion_periodica()
