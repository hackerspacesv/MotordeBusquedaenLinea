/*
 * 
 * This file is part of I32CTT (Integer 32-bit Control & Telemetry Transport).
 * Copyright (C) 2017 Mario Gomez.
 * 
 * I32CTT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * I32CTT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with I32CTT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <string.h>
#include "I32CTT.h"
#include <Arduino.h>

/**
 * \brief Constructor del driver de modo.
 *        El constructor inicializa el modo del driver,
 *        al inicializarse se puede especificar el id
 *        utilizado para seleccionar este modo entre
 *        los demás que puedan estar instanciados en el
 *        mismo controlador.
 *        
 * \param[in] mode_id  Identificador del driver. Nota: 
 *          el controlador no verifica que sea único.
 */
I32CTT_ModeDriver::I32CTT_ModeDriver(uint32_t driver_name) {
  this->driver_name = driver_name;
}

uint32_t I32CTT_ModeDriver::get_name() {
  return this->driver_name;
}

uint32_t I32CTT_ModeDriver::str2name(const char *str) {
  if(strlen(str)!=3)
    return 0;
  uint32_t result = *((uint32_t*)str);

  return result;
}


void I32CTT_Controller::enable_scheduler() {
  this->scheduler_enabled = 1;
}
void I32CTT_Controller::disable_scheduler() {
  this->scheduler_enabled = 0;
}

/**
 * \brief Constructor del controlador.
 *        El constructor del controlador inicializa los
 *        atributos privados utilizados por el mismo.
 *        No recibe ningún parámetro.
 */
I32CTT_Controller::I32CTT_Controller(uint8_t total_modes) {
  this->interface = 0;
  this->total_modes = total_modes;
  this->modes_set = 0;
  this->drivers = (I32CTT_ModeDriver**)malloc((sizeof(I32CTT_ModeDriver*)*total_modes));
  for(int i=0;i<this->total_modes;i++) {
    this->drivers[i] = NULL;
  }
}

/**
 * \brief Establece la interfaz de comunicación.
 *        Este método se encarga de  establecer la interfaz
 *        de comunicaciones a utilizada por el controlador,
 *        por el momento el controlador solo soporta una 
 *        interfaz de clase activa a la vez.
 * \param iface Instancia de I32CTT_Interface a utilizar.
 */
uint8_t I32CTT_Controller::set_interface(I32CTT_Interface &iface) {
  this->interface = &iface;
  this->interface->init();
  Serial.println("Controller initialized.");
  return 0;
}

/**
 * \brief Agrega un nuevo driver de modo.
 *        Este método agrega una nueva instancia del driver de modo
 *        a la lista interna de modos. Espera una instancia de
 *        I32CTT_ModeDriver. Actualmente se soporta un máximo de
 *        63 modos diferentes de funcionamiento. El modo 0 siempre
 *        corresponde al modo "Descanso" (Idle).
 * \param drv Instancia de I32CTT_ModeDriver para agregar a la lista.
 */
uint8_t I32CTT_Controller::add_mode_driver(I32CTT_ModeDriver &drv) {
  if(this->modes_set >= this->total_modes)
    return 0;

  Serial.print("Adding mode at ");
  Serial.println(this->modes_set, DEC);
  this->drivers[this->modes_set++] = &drv;
  
  drv.init();
  
  return this->modes_set;
}

/**
 * \brief Inicializa la clase, interfaz y drivers de modo.
 *        Este método se encarga de inicializar la interfaz como
 *        todos aquellos modos agregados a la lista de modos disponibles
 *        incluyendo "Idle". Esta función no toma parámetros.
 */
void I32CTT_Controller::init() {
  Serial.println("Machine started...");
}

/**
 * \brief Actualiza el estado de ejecución interno.
 *        Run se encarga de realizar varias tareas, entre ellas
 *        hacer la consulta a la interfaz por nuevos datos y llamar
 *        al método "update()" de los drivers de modo.
 *        Aunque está pensado para llamarse de forma no-blocante
 *        en caso de recibirse nuevos paquetes la ejecución
 *        no se libera hasta que el paquete ha sido procesado.
 *        Los paquetes siempre se procesan antes de llamar al
 *        método "update()" es decir que si un paquete solicita
 *        el cambio de driver actual, los registros a leer o
 *        escribir se consultarán sobre el driver de modo solicitado.
 */
void I32CTT_Controller::run() {
  // Look for network events
  if(this->interface != 0) {
    this->interface->update();
    if(this->interface->available()) {
      Serial.println("Data available");
      this->parse(this->interface->rx_buffer, this->interface->rx_size);
    }
  }

  if(this->scheduler_enabled) {
    for(int i=0;i < this->modes_set; i++) {
      if(this->drivers[i] != 0 && this->drivers[i]->enabled()) {
        this->drivers[i]->update();
      }
    }
  }
}

/**
 * \brief Valida el tamaño de los datos recibidos por la interfaz.
 *        Este método valida que el tamaño de los datos recibidos
 *        a travéz de la interfaz de comunicaciones tenga un tamaño
 *        válido en función del comando recibido.
 * \param cmd_type Identificador del tipo de comando.
 * \param buffsize Tamaño del buffer (en bytes).
 */
uint8_t I32CTT_Controller::valid_size(uint8_t cmd_type, uint8_t buffsize) {
  uint8_t result = 0;
  uint8_t min_size = 0;
  
  if(cmd_type == CMD_R || cmd_type == CMD_AW) {
     // Check for header (1-byte) + (1-byte) + [Addr (2-byte) ...]
     min_size = sizeof(I32CTT_Header);
     result = (buffsize>min_size) && ((buffsize-sizeof(I32CTT_Header))%sizeof(I32CTT_Reg) == 0);
     
  } else if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     // Check for header (1-byte) + (1-byte) [(Addr (2-byte) + Data (4-byte) ...]
     min_size = sizeof(I32CTT_Header);
     result = (buffsize>min_size) && ((buffsize-sizeof(I32CTT_Header))%sizeof(I32CTT_RegData) == 0);
  }

  return result;
}

/**
 * \brief Cuenta el número de registros a leer o escribir.
 *        Este método cuenta el número de registros en base al comando
 *        solicitado y el tamaño de los datos recibidos. Este método
 *        permite descartar paquetes que no tienen sentido.
 * \param cmd_type Identificador del tipo de comando.
 * \param buffsize Tamaño del buffer (en bytes).
 */
uint8_t I32CTT_Controller::reg_count(uint8_t cmd_type, uint8_t buffsize) {
  uint8_t result = 0;
  uint8_t header_size = sizeof(I32CTT_Header);
  
  if(cmd_type == CMD_R || cmd_type == CMD_AW) {
     // Check for header (1-byte) + [Addr (4-byte) ...]
     result = (buffsize-header_size)/sizeof(I32CTT_Reg);
  } else if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     // Check for header (1-byte) + [(Addr (4-byte) + Data (4-byte) ...]
     result = (buffsize-header_size)/sizeof(I32CTT_RegData);
  }

  return result;
}

/**
 * \brief Obtiene la dirección del registro en un buffer.
 *        Este método obtiene un registro del buffer especificado.
 *        Importante: Este método no valida si el buffer tiene un
 *        tamaño válido, esto debe verificarse con el método valid_size.
 *        Este método se puede aplicar a cualquier comando.
 * \param buffer Puntero al buffer de bytes con los datos.
 * \param cmd_type Tipo de comando solicitado.
 * \param pos Posición del registro (0..n).
 */
uint16_t I32CTT_Controller::get_reg(uint8_t *buffer, uint8_t cmd_type, uint8_t pos) {
  uint16_t result = 0;
  uint8_t offset = 0;
  uint8_t header_size = sizeof(I32CTT_Header);
  
  if(cmd_type == CMD_R || cmd_type == CMD_AW) {
     offset = header_size+pos*sizeof(I32CTT_Reg);
  } else if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     offset = header_size+pos*sizeof(I32CTT_RegData);
  } else {
    return result;
  }

  memcpy(&result, buffer+offset, sizeof(uint16_t));

  return result;
}

/**
 * \brief Obtiene el dato almacenado en un buffer.
 *        Este método obtiene los datos (un entero de 32 bits) alojado
 *        en un buffer de datos proveniente de la interfaz. Importante:
 *        Este método no valida si el buffer tiene un tamaño válido, esto
 *        debe verificarse con el método valid_size.
 *        El método solo puede proporcionar una respuesta válida
 *        si el comando corresponde a CMD_W o CMD_AR.
 * \param buffer Puntero al buffer de bytes con los datos.
 * \param cmd_type Tipo de comando solicitado.
 * \param pos Posición del registro (0..n).
 */
uint32_t I32CTT_Controller::get_data(uint8_t *buffer, uint8_t cmd_type, uint8_t pos) {
  uint32_t result = 0;
  uint8_t offset = 0;
  uint8_t header_size = sizeof(I32CTT_Header);
  
  if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     offset = header_size+sizeof(I32CTT_Reg)+pos*sizeof(I32CTT_RegData);
  } else {
    return result;
  }

  memcpy(&result, buffer+offset, sizeof(uint32_t));

  return result;
}
/**
 * \brief Empuja una direccion de registro al buffer de escritura.
 *        Este método establece un valor de registro dentro del buffer
 *        de escritura. Este comando es válido para cualquier tipo
 *        de comando. Es utilizado para preparar los paquetes
 *        a enviarse por la interfaz.
 * \param buffer Puntero al buffer de bytes con los datos.
 * \param reg Valor del registro solicitado
 * \param cmd_type Tipo de comando solicitado.
 * \param pos Posición del registro (0..n).
 */
void I32CTT_Controller::put_reg(uint8_t *buffer, uint16_t reg, uint8_t cmd_type, uint8_t pos) {
  uint8_t offset = 0;
  uint8_t header_size = sizeof(I32CTT_Header);
  
  if(cmd_type == CMD_R || cmd_type == CMD_AW) {
     offset = header_size+pos*sizeof(I32CTT_Reg);
  } else if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     offset = header_size+pos*sizeof(I32CTT_RegData);
  } else {
    return;
  }

  memcpy(buffer+offset, &reg, sizeof(uint16_t));
}

/**
 * \brief Empuja un dato al buffer de escritura.
 *        Este método establece un dato dentro del buffer
 *        de escritura. Este comando es válido únicamente para
 *        los comando CMD_W y CMD_AR. Es utilizado para preparar
 *        los paquetes a enviarse por la interfaz.
 * \param buffer Puntero al buffer de bytes con los datos.
 * \param reg Valor del registro solicitado
 * \param cmd_type Tipo de comando solicitado.
 * \param pos Posición del registro (0..n).
 */
void I32CTT_Controller::put_data(uint8_t *buffer, uint32_t data, uint8_t cmd_type, uint8_t pos) {
  uint8_t offset = 0;
  uint8_t header_size = sizeof(I32CTT_Header);
  
  if (cmd_type == CMD_W || cmd_type == CMD_AR) {
     offset = header_size+sizeof(I32CTT_Reg)+pos*sizeof(I32CTT_RegData);
  } else {
    return;
  }

  memcpy(buffer+offset, &data, sizeof(uint32_t));
}

/**
 * \brief Procesa los datos recibidos por la interfaz.
 *        Este método se encarga de procesar todos los datos recibidos
 *        a través de la interfaz. La interfaz tiene como única función
 *        extraer los datos útiles del medio y protocolo que maneje
 *        internamente. Es esta función donde se valida el formato
 *        y se ejecutan los comandos acorde.
 * \param buffer Puntero al buffer de datos.
 * \param buffsize Tamaño del buffer.
 */
void I32CTT_Controller::parse(uint8_t *buffer, uint8_t buffsize) {
  Serial.println("Trying to parse");
  if(buffsize==0) // Should never happend. But here just in case.
    return;
  if(buffsize<sizeof(I32CTT_Header)) // This neither.
    return;

  uint8_t cmd = buffer[0]>>1;
  uint8_t ping = buffer[0]&0x01; // TODO: Get from queue
  uint8_t mode = buffer[1];

  Serial.print("CMD: ");
  Serial.print(cmd, HEX);
  Serial.print("\r\n");
  Serial.print("MODE: ");
  Serial.print(mode, HEX);
  Serial.print("\r\n");

  if(!valid_size(cmd, buffsize)) // return if size invalid
    return;
  Serial.println("Valid size.");

  uint8_t records = reg_count(cmd, buffsize);
  Serial.print("Records: ");
  Serial.print(records, DEC);
  Serial.print("\r\n");

  if((this->interface != NULL) && (this->drivers[mode] !=  NULL)) {
    Serial.println("Calling driver");
    I32CTT_ModeDriver *driver = this->drivers[mode];
    
    switch(cmd) {
      case CMD_R:
        Serial.print("Buffer size: ");
        Serial.println(buffsize, DEC);
        Serial.print("Records: ");
        Serial.println(records, DEC);
        this->interface->tx_size = sizeof(I32CTT_Header)+records*sizeof(I32CTT_RegData);
        this->interface->tx_buffer[0] = CMD_AR<<1 | ping;
        this->interface->tx_buffer[1] = mode;
        
        for(int i=0;i<records;i++) {
          uint32_t reg = get_reg(buffer, cmd, i);
          Serial.print("Register: ");
          Serial.println(reg, DEC);
          put_reg(this->interface->tx_buffer, reg, CMD_AR, i);
          put_data(this->interface->tx_buffer, driver->read(reg) , CMD_AR, i);
        }

        this->interface->send();
        break;
      case CMD_AR:
        // Forward to response handler
        break;
      case CMD_W:
        Serial.print("Buffer size: ");
        Serial.println(buffsize, DEC);
        Serial.print("Records: ");
        Serial.println(records, DEC);
        this->interface->tx_size = sizeof(I32CTT_Header)+records*sizeof(I32CTT_Reg);
        this->interface->tx_buffer[0] = CMD_AW<<1 | ping;
        this->interface->tx_buffer[1] = mode;

        for(int i=0;i<records;i++) {
          uint32_t reg = get_reg(buffer, cmd, i);
          uint32_t data = get_data(buffer, cmd, i);
          Serial.print("Register: ");
          Serial.println(reg, DEC);
          driver->write(reg, data);
          put_reg(this->interface->tx_buffer, reg, CMD_AW, i);
        }

        this->interface->send();
        break;
      case CMD_AW:
        // Forward to response handler
        break;
      default:
        break;
    }
  }
}

