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
  reg_fd = 6
  reg_rx = 7
  reg_ry = 8
  reg_nv_slot = 9
  reg_nv_vc = 10
  reg_nv_kp = 11
  reg_nv_kd = 12
  reg_nv_fd = 13
  reg_nv_rx = 14
  reg_nv_ry = 15

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
    ajuste = []
    ajuste.append({'escala': +1.0,   'pos_hint': {'x': 0.7, 'top': 0.7}})
    ajuste.append({'escala': -1.0,   'pos_hint': {'x': 0.8, 'top': 0.7}})
    ajuste.append({'escala': +0.1,   'pos_hint': {'x': 0.7, 'top': 0.6}})
    ajuste.append({'escala': -0.1,   'pos_hint': {'x': 0.8, 'top': 0.6}})
    ajuste.append({'escala': +0.01,  'pos_hint': {'x': 0.7, 'top': 0.5}})
    ajuste.append({'escala': -0.01,  'pos_hint': {'x': 0.8, 'top': 0.5}})
    ajuste.append({'escala': +0.001, 'pos_hint': {'x': 0.7, 'top': 0.4}})
    ajuste.append({'escala': -0.001, 'pos_hint': {'x': 0.8, 'top': 0.4}})

    for i in ajuste:
      #Se crea el boton
      b = Button(text = '{:+}'.format(i['escala']), size_hint = (0.1, 0.1), pos_hint = i['pos_hint'])
      #Se anexa al boton la informacion de la escala asociada
      b.escala = i['escala']
      #Se asocia el evento de presion del hijo a la funcion correspondiente
      b.bind(on_press = self.__ajustar_parametro)
      #Se anexa el widget a este layout
      self.add_widget(b)

    #Lista de parametros
    param = []
    param.append({'text': 'Vc', 'reg_act': self.reg_vc, 'reg_sal': self.reg_nv_vc, 'pos_hint': {'x': 0, 'top': 0.9}})
    param.append({'text': 'Kp', 'reg_act': self.reg_kp, 'reg_sal': self.reg_nv_kp, 'pos_hint': {'x': 0, 'top': 0.8}})
    param.append({'text': 'Kd', 'reg_act': self.reg_kd, 'reg_sal': self.reg_nv_kd, 'pos_hint': {'x': 0, 'top': 0.7}})
    param.append({'text': 'Fd', 'reg_act': self.reg_fd, 'reg_sal': self.reg_nv_fd, 'pos_hint': {'x': 0, 'top': 0.6}})
    param.append({'text': 'Rx', 'reg_act': self.reg_rx, 'reg_sal': self.reg_nv_rx, 'pos_hint': {'x': 0, 'top': 0.5}})
    param.append({'text': 'Ry', 'reg_act': self.reg_ry, 'reg_sal': self.reg_nv_ry, 'pos_hint': {'x': 0, 'top': 0.4}})

    self.layout_param = []
    for i in param:
      #Se crea el layout
      l = LayoutParametro(text = i['text'], size_hint = (0.5, 0.1), pos_hint = i['pos_hint'])
      self.layout_param.append(l)
      #Se anexa al layout la informacion de los registros asociados
      l.reg_act = i['reg_act']
      l.reg_sal = i['reg_sal']
      #Se asocia el evento de seleccion del hijo a la funcion correspondiente
      l.bind(on_seleccion = self.__parametro_seleccionado)

      l.bind(on_salvar = self.__salvar_parametro)
      l.bind(on_restaurar = self.__restaurar_parametro)

      #Se anexa el widget a este layout
      self.add_widget(l)

    #Se marca el primer layout como seleccionado
    self.layout_param[0].bot_sel.state = 'down'
    self.prm_seleccionado = self.layout_param[0]

    #Se anexa elevento de seleccion de este layout a la funcion correspondiente
    self.bind(seleccionado = self.__actualizar_seleccion)

    slot = []
    slot.append({'pos_hint': {'x': 0.1, 'y': 0}})
    slot.append({'pos_hint': {'x': 0.2, 'y': 0}})
    slot.append({'pos_hint': {'x': 0.3, 'y': 0}})
    slot.append({'pos_hint': {'x': 0.4, 'y': 0}})

    self.boton_slot = []
    for i in slot:
      #Se crea el boton
      b = ToggleButton(text = str(len(self.boton_slot)), size_hint = (0.1, 0.1),
                       pos_hint = i['pos_hint'], group = 'slot')
      #Se anexa al boton la informacion del numero de slot
      b.num_slot = len(self.boton_slot)
      #Se asocia el evento de presion del hijo a la funcion correspondiente
      b.bind(on_press = self.__seleccionar_slot)
      #Se anexa el widget a este layout
      self.boton_slot.append(b)
      self.add_widget(b)

  def __actualizar_seleccion(self, *args):
    #Se prepara una lista con todas las direcciones a leer
    direcciones = map(lambda a:a.reg_act, self.layout_param)

    #Se efectua el proceso de lectura de los registros
    pares = self.i32ctt.leer_registro(self.direccion_mac, self.endpoint, direcciones)
    if pares:
      #Si hubo respuesta, se obtiene la lista de direcciones leidas
      direcciones_leidas = map(lambda a:a[0], pares)
      if direcciones_leidas == direcciones:
        #Si las direcciones coinciden, se obtiene la lista de datos leidos
        datos_leidos = map(lambda a:a[1], pares)

        #Se aplican los datos leidos en el orden correlativo que se recibieron
        for i in range(len(self.layout_param)):
          self.layout_param[i].valor_act = round(float(datos_leidos[i]) / (1 << 24), 3)

    direcciones = map(lambda a:a.reg_sal, self.layout_param)
    pares = self.i32ctt.leer_registro(self.direccion_mac, self.endpoint, direcciones)
    if pares:
      direcciones_leidas = map(lambda a:a[0], pares)
      if direcciones_leidas == direcciones:
        datos_leidos = map(lambda a:a[1], pares)
        for i in range(len(self.layout_param)):
          self.layout_param[i].valor_sal = round(float(datos_leidos[i]) / (1 << 24), 3)

    direccion = (self.reg_nv_slot,)
    pares = self.i32ctt.leer_registro(self.direccion_mac, self.endpoint, direccion)
    if pares:
      direccion_leida = pares[0][0]
      if direccion_leida == direccion[0]:
        dato_leido = pares[0][1]
        if dato_leido < len(self.boton_slot):
          self.boton_slot[dato_leido].state = 'down'

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

  def __salvar_parametro(self, parametro):
    registro = parametro.reg_sal
    valor = int(parametro.valor_act * (1 << 24))

    direcciones_escritas = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,
                                                     ((registro, valor),))
    if direcciones_escritas:
      if direcciones_escritas[0] == registro:
        parametro.valor_sal = parametro.valor_act

  def __restaurar_parametro(self, parametro):
    registro = parametro.reg_act
    valor = int(parametro.valor_sal * (1 << 24))

    direcciones_escritas = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,
                                                     ((registro, valor),))
    if direcciones_escritas:
      if direcciones_escritas[0] == registro:
        parametro.valor_act = parametro.valor_sal

  def __ajustar_parametro(self, boton):
    self.prm_seleccionado.valor_act = round(self.prm_seleccionado.valor_act + boton.escala, 3)

    registro = self.prm_seleccionado.reg_act
    valor = int(self.prm_seleccionado.valor_act * (1 << 24))

    registros = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,
                                          ((registro, valor),))

  def __seleccionar_slot(self, boton):
    registro = self.reg_nv_slot
    valor = boton.num_slot

    registros = self.i32ctt.escr_registro(self.direccion_mac, self.endpoint,
                                          ((registro, valor),))

    direcciones = map(lambda a:a.reg_sal, self.layout_param)
    pares = self.i32ctt.leer_registro(self.direccion_mac, self.endpoint, direcciones)
    if pares:
      direcciones_leidas = map(lambda a:a[0], pares)
      if direcciones_leidas == direcciones:
        datos_leidos = map(lambda a:a[1], pares)
        for i in range(len(self.layout_param)):
          self.layout_param[i].valor_sal = round(float(datos_leidos[i]) / (1 << 24), 3)

class LayoutParametro(BoxLayout):
  valor_act = NumericProperty(0)
  valor_sal = NumericProperty(0)

  def __init__(self, **kwargs):
    self.register_event_type('on_seleccion')
    self.register_event_type('on_salvar')
    self.register_event_type('on_restaurar')
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
    self.bot_sal.bind(on_press = self.__salvado)

    self.bot_res = Button(text = 'Restaurar')
    self.add_widget(self.bot_res)
    self.bot_res.bind(on_press = self.__restaurado)

    self.bind(valor_act = self.__valor_cambiado)
    self.bind(valor_sal = self.__valor_cambiado)

  def __seleccionado(self, boton):
    if boton.state == 'down':
      self.dispatch('on_seleccion')
    else:
      boton.state = 'down'

  def __salvado(self, boton):
    self.dispatch('on_salvar')

  def __restaurado(self, boton):
    self.dispatch('on_restaurar')

  def __valor_cambiado(self, *args):
    self.txt_valor_act.text = str(self.valor_act)
    self.txt_valor_sal.text = str(self.valor_sal)

  def on_seleccion(self, *args):
    pass

  def on_salvar(self, *args):
    pass

  def on_restaurar(self, *args):
    pass

  #def valor_act(self):
    #return self.txt_valor_act.text

  #def valor_sal(self):
    #return self.txt_valor_sal.text
