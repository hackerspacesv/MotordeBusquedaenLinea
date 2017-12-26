from kivy.uix.boxlayout import BoxLayout
from kivy.uix.button import Button
from kivy.uix.togglebutton import ToggleButton
from kivy.uix.label import Label
from kivy.properties import NumericProperty

from mbl_layout_endpoint import LayoutEndpoint

class LayoutEndpointAutomatico(LayoutEndpoint):
  reg_comando = 2
  reg_vc = 3
  reg_kp = 4
  reg_kd = 5

  def __init__(self, i32ctt, direccion_mac, endpoint, **kwargs):
    super(LayoutEndpointAutomatico, self).__init__(i32ctt, direccion_mac, endpoint, **kwargs)

    #Se crean los botones
    self.boton_arranque = Button(text = 'Arrancar',
                                 size_hint = (0.1, 0.1), pos_hint = {'right': 0.8, 'top': 0.9});
    self.boton_calibracion = Button(text = 'Calibrar',
                                    size_hint = (0.1, 0.1), pos_hint = {'right': 0.9, 'top': 0.9});
    self.boton_paro = Button(text = 'Parar',
                             size_hint = (0.1, 0.1), pos_hint = {'right': 1.0, 'top': 0.9});

    #Se agregan los botones al layout
    self.add_widget(self.boton_arranque)
    self.add_widget(self.boton_calibracion)
    self.add_widget(self.boton_paro)

    #Se asocian los eventos de click de los botones con la funcion correspondiente
    self.boton_arranque.bind(on_press = self.__boton_presionado)
    self.boton_calibracion.bind(on_press = self.__boton_presionado)
    self.boton_paro.bind(on_press = self.__boton_presionado)

    #Etiquetas fijas
    self.add_widget(Label(text = 'Actual',
                          size_hint = (0.1, 0.1), pos_hint = {'x': 0.1, 'top': 1.0}))
    self.add_widget(Label(text = 'Guardado',
                          size_hint = (0.1, 0.1), pos_hint = {'x': 0.2, 'top': 1.0}))

    #Teclas de ajuste numerico
    self.p_1   = Button(text = '+1.0',  size_hint = (0.1, 0.1), pos_hint = {'x': 0.7, 'top': 0.7})
    self.m_1   = Button(text = '-1.0',  size_hint = (0.1, 0.1), pos_hint = {'x': 0.8, 'top': 0.7})
    self.p_01  = Button(text = '+0.1',  size_hint = (0.1, 0.1), pos_hint = {'x': 0.7, 'top': 0.6})
    self.m_01  = Button(text = '-0.1',  size_hint = (0.1, 0.1), pos_hint = {'x': 0.8, 'top': 0.6})
    self.p_001 = Button(text = '+0.01', size_hint = (0.1, 0.1), pos_hint = {'x': 0.7, 'top': 0.5})
    self.m_001 = Button(text = '-0.01', size_hint = (0.1, 0.1), pos_hint = {'x': 0.8, 'top': 0.5})

    self.add_widget(self.p_1)
    self.add_widget(self.m_1)
    self.add_widget(self.p_01)
    self.add_widget(self.m_01)
    self.add_widget(self.p_001)
    self.add_widget(self.m_001)

    self.p_1.bind(on_press = self.__ajustar_parametro)
    self.m_1.bind(on_press = self.__ajustar_parametro)
    self.p_01.bind(on_press = self.__ajustar_parametro)
    self.m_01.bind(on_press = self.__ajustar_parametro)
    self.p_001.bind(on_press = self.__ajustar_parametro)
    self.m_001.bind(on_press = self.__ajustar_parametro)

    #Parametros individuales
    self.prm_vc = LayoutParametro(text = 'Vc',
                                  size_hint = (0.5, 0.1), pos_hint = {'x': 0, 'top': 0.9})
    self.prm_kp = LayoutParametro(text = 'Kp',
                                  size_hint = (0.5, 0.1), pos_hint = {'x': 0, 'top': 0.8})
    self.prm_kd = LayoutParametro(text = 'Kd',
                                  size_hint = (0.5, 0.1), pos_hint = {'x': 0, 'top': 0.7})

    self.add_widget(self.prm_vc)
    self.add_widget(self.prm_kp)
    self.add_widget(self.prm_kd)

    self.prm_vc.bind(on_seleccion = self.__parametro_seleccionado)
    self.prm_kp.bind(on_seleccion = self.__parametro_seleccionado)
    self.prm_kd.bind(on_seleccion = self.__parametro_seleccionado)

    self.prm_vc.bot_sel.state = 'down'
    self.prm_seleccionado = self.prm_vc

    self.bind(seleccionado = self.__actualizar_seleccion)

  def __actualizar_seleccion(self, *args):
    registros = self.i32ctt.leer_registro(self.direccion_mac, self.endpoint,
                                          (self.reg_vc, self.reg_kp, self.reg_kd))
    if registros and registros[0][0] == self.reg_vc and registros[1][0] == self.reg_kp and\
       registros[2][0] == self.reg_kd:
      self.prm_vc.valor = round(float(registros[0][1]) / (1 << 24), 2)
      self.prm_kp.valor = round(float(registros[1][1]) / (1 << 24), 2)
      self.prm_kd.valor = round(float(registros[2][1]) / (1 << 24), 2)

  def __boton_presionado(self, boton):
    if boton is self.boton_arranque:
      registros = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,
                                            ((self.reg_comando, 1),))

    if boton is self.boton_calibracion:
      registros = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,
                                            ((self.reg_comando, 2),))

    if boton is self.boton_paro:
      registros = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,
                                            ((self.reg_comando, 0),))

  def __parametro_seleccionado(self, parametro):
    self.prm_seleccionado = parametro

  def __ajustar_parametro(self, boton):
    if boton is self.p_1:
      self.prm_seleccionado.valor = round(self.prm_seleccionado.valor + 1, 2)
    if boton is self.m_1:
      self.prm_seleccionado.valor = round(self.prm_seleccionado.valor - 1, 2)
    if boton is self.p_01:
      self.prm_seleccionado.valor = round(self.prm_seleccionado.valor + 0.1, 2)
    if boton is self.m_01:
      self.prm_seleccionado.valor = round(self.prm_seleccionado.valor - 0.1, 2)
    if boton is self.p_001:
      self.prm_seleccionado.valor = round(self.prm_seleccionado.valor + 0.01, 2)
    if boton is self.m_001:
      self.prm_seleccionado.valor = round(self.prm_seleccionado.valor - 0.01, 2)

    if self.prm_seleccionado is self.prm_vc:
      valor = int(self.prm_vc.valor * (1 << 24))
      registro = self.reg_vc
    if self.prm_seleccionado is self.prm_kp:
      valor = int(self.prm_kp.valor * (1 << 24))
      registro = self.reg_kp
    if self.prm_seleccionado is self.prm_kd:
      valor = int(self.prm_kd.valor * (1 << 24))
      registro = self.reg_kd

    registros = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,
                                          ((registro, valor),))

class LayoutParametro(BoxLayout):
  valor = NumericProperty(0)

  def __init__(self, **kwargs):
    self.register_event_type('on_seleccion')
    super(LayoutParametro, self).__init__(orientation = 'horizontal',
                                          size_hint = kwargs['size_hint'],
                                          pos_hint = kwargs['pos_hint'])

    self.bot_sel = ToggleButton(text = kwargs['text'], group = 'parametro')
    self.add_widget(self.bot_sel)
    self.bot_sel.bind(on_press = self.__seleccionado)

    self.txt_valor_act = Label(text = '0.0')
    self.add_widget(self.txt_valor_act)

    self.txt_valor_sal = Label(text = '0.0')
    self.add_widget(self.txt_valor_sal)

    self.bot_sal = Button(text = 'Salvar')
    self.add_widget(self.bot_sal)

    self.bot_res = Button(text = 'Restaurar')
    self.add_widget(self.bot_res)

    self.bind(valor = self.__valor_cambiado)

  def __seleccionado(self, boton):
    if boton.state == 'down':
      self.dispatch('on_seleccion')
    else:
      boton.state = 'down'

  def __valor_cambiado(self, *args):
    self.txt_valor_act.text = str(self.valor)

  def on_seleccion(self, *args):
    pass

  def valor_act(self):
    return self.txt_valor_act.text

  def valor_sal(self):
    return self.txt_valor_sal.text
