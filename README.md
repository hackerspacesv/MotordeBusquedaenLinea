Motor de Búsqueda en Línea
==========================
**Motor de Búsqueda en Línea** es el robot ganador del 2do Lugar en la competencia de seguidores de líneas en el ArduChallenge organizado por la <a href="https://www.facebook.com/ArduinoGuatemala/">Comunidad Arduino de Guatemala</a> el 27 de diciembre de 2017.

MBL es un proyecto de <a href="https://www.oshwa.org/definition/spanish/">Hardware Libre</a>. Todo el código fuente, archivos de diseño y documentación está disponible en este repositorio para cualquiera que desee utilizarlo o crear proyectos derivados del mismo.

Este proyecto ha sido desarrollado gracias a los aportes de Joksan Alvarado, Mario Gómez y Karla Hernández del <a href="http://hackerspace.teubi.co/">Hackerspace San Salvador</a>. El equipo del Hackerspace espera que este proyecto pueda ser replicado y utilizado para fines educativos en donde sea necesario.

¿Qué contiene este repositorio?
-------------------------------
El repositorio de MBL está organizado de la siguiente manera en las diferentes carpetas:

- **Datasheets**: Este directorio contiene las hojas técnicas de los chips utilizados en el diseño electrónico de MBL. 

- **Design**: Incluye todos los archivos de diseño mecánico y electrónico utilizados para la construcción del robot. Los archivos de diseño electrónico se pueden editar con <a href="https://www.autodesk.com/products/eagle/overview">EAGLE de Autodesk</a> y los archivos de diseño mecánico se pueden editar con <a href="https://www.freecadweb.org/">FreeCAD</a>. Esta carpeta también incluye las listas de materiales o BOM.

- **Documentation**: Esta carpeta contiene toda la documentación referente al código fuente provisto con MBL. La carpeta incluye desde notas de diseño, parámetros de configuración de protocolos de comunicación y la documentación de APIs para programadores.

- **Libraries**: Esta carpeta contienen bibliotecas de componentes utilizadas en los diseños de CAD.

- **Source**: Contiene los archivos de código fuente de la API para programadores y ejemplos básicos de uso de la misma para diferentes plataformas.

Los sub-directorios de cada carpeta podrían contener archivos *README.md* con información importante sobre el contenido de la misma.

¿Cómo utilizar MBL?
-------------------
Esta es una guía muy rápida (e incompleta) sobre cómo hacer construir y hacer funcionar a MBL.

**Hardware**:
>1. Adquiere todos los componentes especificados en el archivo *BOM.odx* en la carpeta *Design*.
>2. Corta el acrílico del chasis según los archivos de diseño provistos en la carpeta *Design/FreeCAD/SVG corte laser*.
>3. Fabrica las PCBs y ensambla según archivos de diseño en *Design/eagle*.
>4. Conecta todo según especificaciones y revisa que las conexiones eléctricas no estén cruzadas o contengan falso-contactos.

**Software**:
>1. Instala el soporte para Teensy con <a href="https://www.pjrc.com/teensy/td_download.html">Teensyduino</a>.
>2. Copia la carpeta *Source/Arduino/libraries/I32CTT* a tu carpeta de bibliotecas en Arduino.
>3. Utiliza el Sketch de ejemplo "Source/Arduino/I32CTT_PoC" para programar el robot.
>4. Utiliza el Sketch de ejemplo "Source/Arduino/I32CTT_PoC_Master" para programar controles remotos.

Si hasta este punto nada ha tomado fuego podrás utilizar MBL a través de control remoto. Modifica los ejemplos provistos para agregar funcionalidad adicional.

En la carpeta "Source/python" encontrarás también ejemplos de aplicación de control y comunitación al robot. Para ello necesitas una <a href="https://www.raspberrypi.org/">Raspberry Pi</a> y un <a href="http://openlabs.co/OSHW/Raspberry-Pi-802.15.4-radio">radio 802.15.4 de OpenLabs.co</a>.

Tecnologías utilizadas
----------------------
MBL no solo pretende ser un robot de uso específico sino una plataforma para la prueba y el desarrollo de soluciones del <a href="https://es.wikipedia.org/wiki/Internet_de_las_cosas">Internet de las Cosas (IoT)</a>. Por ello MBL hace uso de las siguientes tecnologías:

* **Procesamiento con ARM-Cortex**: <a href="https://www.arm.com/">ARM</a> es la arquitectura más utilizada en el desarrollo no solo de dispositivos móviles sino también de soluciones de IoT. MBL procesa toda la información utilizando la tarjeta electrónica <a href="https://www.pjrc.com/teensy/teensy31.html">PJRC Teensy 3.2</a> que utiliza un poderoso microcontrolador de <a href="https://www.nxp.com/products/processors-and-microcontrollers/arm-based-processors-and-mcus/kinetis-cortex-m-mcus/k-seriesperformancem4/k2x-usb/kinetis-k20-100-mhz-full-speed-usb-mixed-signal-integration-microcontrollers-based-on-arm-cortex-m4-core:K20_100">32-bits ARM Cortex-M4</a> corriendo a 96Mhz.
* **Comunicaciones inalámbricas con IEEE 802.15.4**: El estándar <a href="https://en.wikipedia.org/wiki/IEEE_802.15.4">IEEE 802.15.4</a> es la apuesta de <a href="https://www.ieee.org/index.html">IEEE</a> para las soluciones de IoT. Este protocolo de corto alcance y bajo ancho de banda resulta ideal para dispositivos con bajo poder de procesamiento y que requieren de un consumo mínimo de energía.
* **Control remoto y Telemetría con I32CTT**: Este protocolo desarrollado en casa por el Hackerspace San Salvador combina una especificación de mensajes cortos utilizando números enteros de 32 bits que puede ser transmitido sobre diferentes medios de comunicación. Actualmente se soportan puentes serie bluetooth e IEEE 802.15.4.

Licenciamiento
--------------
A menos que se especifique lo contrario en el archivo de código fuente, todo el código fuente incluído como parte de MBL se distribuye bajo la licencia <a href="https://www.gnu.org/licenses/gpl-3.0.en.html">GNU GPL v3.0</a>.

Todos los archivos de diseño mecánico, diseño electrónico y la documentación se distribuyen bajo la licencia Creative-Commons Attribution-ShareAlike 2.0 Generic (<a href="https://creativecommons.org/licenses/by-sa/2.0/">CC BY-SA 2.0</a>).

Archivos de documentación de terceros como hojas de datos u otros se incluyen en el repositorio sólo con fines informativos y los copyrights corresponden a sus respectivos dueños.

Todos los nombres de marcas y tecnologías mencionados en este README y documentación adjunta son propiedad de sus respectivos dueños y se utilizan solo con fines informativos.

Lo anterior implica que puedes copiar/modificar y redistribuir MBL para fines comerciales y no comerciales en tanto incluyas la información original de los autores y utilices licencias compatibles a las mencionadas anteriormente.

Información de contacto
-----------------------
Si deseas agregar funcionalidad adicional a MBL puedes enviar un *"pull request"* a este repositorio. De igual manera si encuentras algo que no funciona envíanos un *"issue"* para que podamos realizar las correcciones.

Si estás interesado en colaborar en algo más que código fuente y diseño como por ejemplo: Generar material educativo, implementar MBL en tu centro de estudios o realizar publicaciones o investigaciones tomando como base este proyecto, puedes contactarte con el equipo de desarrollo al correo electrónico <a href="mailto:info@teubi.co">info@teubi.co</a>.
