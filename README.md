# 2026-f2-receptores-radio-fm
Taller de Proyecto II 2026 — Grupo F2 — Receptores de Radio FM HEX3653 y TEA5767

# 📻 Receptores de Radio FM Dual (TEA5767 & HEX3653) — ESP8266

![ESP8266](https://img.shields.io/badge/MCU-ESP8266--NodeMCU%20v3-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Arduino-green)
![Web Interface](https://img.shields.io/badge/Interface-HTML5%20%2F%20CSS3%20%2F%20JS-orange)
![Academic](https://img.shields.io/badge/UNLP-Taller%20de%20Proyecto%20II-red)

Este repositorio contiene el firmware y la documentación técnica para el **Sistema de Radio FM Dual** desarrollado para la asignatura **Taller de Proyecto II** de la Facultad de Informática de la **Universidad Nacional de La Plata (UNLP)**.

El sistema integra dos receptores de radio de distinta arquitectura: el **TEA5767** y el **HEX3653**. La selección de canal se realiza mediante un módulo de relé electromecánico **HL-51**, con control digital de volumen a través del potenciómetro **X9C103S**, amplificación con la etapa **PAM8403** y gestión remota mediante una interfaz web embebida servida en un Access Point propio generado por el **ESP8266**.

---

## 📂 Estructura del Repositorio

2026-f2-receptores-radio-fm/
├── F2ProyectoFinal.ino       # Programa principal (setup, loop y comandos por monitor serie)
├── HTTPGeneral.cpp / .h       # Servidor HTTP, configuración del AP WiFi y control del relé HL-51
├── HandlerTEA5767.cpp / .h    # Controlador y rutas HTTP para el módulo TEA5767
├── HandlerHEX3653.cpp / .h    # Emulación de pulsos GPIO y rutas HTTP para el HEX3653
├── HandlerX9C103S.cpp / .h    # Gestión del potenciómetro digital X9C103S y volumen
├── InterfazWeb.h             # Página web interactiva (HTML, CSS, JS asincrónico en PROGMEM)
├── TEA5767.cpp / .h           # Librería núcleo para la comunicación I2C con el TEA5767
└── README.md                 # Documentación principal del proyecto

---

## 🛠️ Componentes y Materiales

| Componente | Función / Descripción | Fuente de Alimentación |
| :--- | :--- | :--- |
| **ESP8266 (NodeMCU v3)** | Microcontrolador principal y Servidor Web AP | +5V DC (VIN) |
| **TEA5767** | Receptora FM digital controlada vía bus I2C | +3.3V DC (regulados) |
| **HEX3653** | Receptora FM con sintonía por impulsos GPIO | +3.3V DC (regulados) |
| **Módulo Relé HL-51** *(Songle 5V)* | Conmutador mecánico para la selección analógica de audio | +5V DC ($V_{CC}$ a VIN, IN a D0) |
| **X9C103S** | Potenciómetro digital de $10\,\text{k}\Omega$ (100 pasos) | +5V DC |
| **PAM8403** | Amplificador estéreo Clase D ($3\text{W} + 3\text{W}$) | +5V DC |
| **Parlante Dinámico** | Reproducción de audio ($8\,\Omega$ / $3\text{W}$) | — |
| **Fuente Externa** | Fuente conmutada regulada de 5V DC / 2A | Red eléctrica |

---

## 🔌 Esquema de Conexiones Físicas (Pinout)

### Mapeo desde el ESP8266 NodeMCU v3

* **Receptor TEA5767 (I2C):**
  * `D1` (GPIO5) $\rightarrow$ **SCL**
  * `D2` (GPIO4) $\rightarrow$ **SDA**
* **Receptor HEX3653 (GPIO):**
  * `D4` (GPIO2) $\rightarrow$ **PSW** (Encendido / Apagado)
  * `D3` (GPIO0) $\rightarrow$ **SEEK-**
  * `D8` (GPIO15) $\rightarrow$ **SEEK+**
* **Módulo Relé HL-51 (Selección de Audio):**
  * $V_{CC} \rightarrow$ **+5V DC (VIN)**
  * $GND \rightarrow$ **GND**
  * $IN \rightarrow$ **`D0` (GPIO16)** del ESP8266
  * **COM (Común):** Conectado a la entrada **VH** del X9C103S.
  * **NO (Normal Open):** Salida de audio del **TEA5767** (se conecta al activar relé, `D0 = LOW`).
  * **NC (Normal Closed):** Salida de audio del **HEX3653** (conectado por defecto en reposo).
* **Potenciómetro Digital X9C103S:**
  * `D5` (GPIO14) $\rightarrow$ **INC** (Pulso de incremento)
  * `D6` (GPIO12) $\rightarrow$ **U/D** (Dirección)
  * `D7` (GPIO13) $\rightarrow$ **CS** (Chip Select)
  * **VH** $\rightarrow$ Terminal **COM** del relé HL-51.
  * **VL** $\rightarrow$ **GND**
  * **VW (Wiper)** $\rightarrow$ Entrada **L-IN** del PAM8403
* **Amplificador PAM8403:**
  * **L-IN** $\rightarrow$ Terminal **VW** del X9C103S
  * **L+ / L-** $\rightarrow$ Parlante de $8\,\Omega$ / $3\text{W}$

> ⚠️ **Nota Técnica:** Para evitar problemas de booteo en el ESP8266, la señal `IN` del módulo relé utiliza `D0`. El relé se desactiva poniendo el pin en modo `INPUT` (el pull-up del módulo lleva `IN` a 5V) y se activa configurando `D0` como `OUTPUT` en `LOW`.

---

## 🚀 Instalación y Carga del Firmware

### Requisitos Previos
1. Descargar e instalar el **Arduino IDE** (v1.8.x o v2.x).
2. Configurar la URL del gestor de tarjetas para ESP8266 en `Archivo` > `Preferencias`:
   `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
3. Instalar la plataforma **esp8266** desde `Herramientas` > `Placa` > `Gestor de Tarjetas`.

### Carga del Proyecto
1. Clonar este repositorio:
   ```bash
   git clone [https://github.com/tpII/2026-f2-receptores-radio-fm.git](https://github.com/tpII/2026-f2-receptores-radio-fm.git)

2. Abrir el archivo F2ProyectoFinal.ino en el Arduino IDE.

3. Configurar la placa en el menú Herramientas:

  * Placa: NodeMCU 1.0 (ESP-12E Module)

  * CPU Frequency: 80 MHz

  * Flash Size: 4MB (FS:2MB)

  * Puerto: Seleccionar el puerto COM asignado al ESP8266.

4. Conectar la placa por USB y presionar Subir (Ctrl + U).

---

### 📱 Guía de Uso de la Interfaz Web

```markdown
## 📱 Guía de Uso de la Interfaz Web

1. **Conexión al Punto de Acceso (AP):**
   * Conectarse a la red WiFi emitida por la placa:
   * **SSID:** `TP2F2_AP`
   * **Contraseña:** `12345678`
2. **Acceso a la Consola:**
   * Abrir un navegador e ingresar a la dirección IP fija: `http://10.10.10.10`
3. **Controles Disponibles:**
   * **Selector de Radio:** Alterna entre **HEX3653** (relé en reposo) y **TEA5767** (relé activado).
   * **Control de Volumen:** Deslizador porcentual (0 a 100%) y botón *Mute*.
   * **Panel TEA5767:** Búsqueda automática (*Seek*), sintonía fina de $\pm 0.1\,\text{MHz}$, ingreso directo de frecuencia e indicador visual de nivel de señal.
   * **Panel HEX3653:** Botones para enviar impulsos `S_UP` (D8), `S_DOWN` (D3) y `ON/OFF` (D4).

---

## 🖥️ Comandos de Control por Monitor Serie

Con el **Monitor Serie** abierto a **115200 baudios**, se pueden enviar comandos directos:

* `t` : Conmuta el relé hacia la radio **TEA5767** (`D0 = LOW`).
* `h` : Conmuta el relé hacia la radio **HEX3653** (`D0 = INPUT`).
* `0` a `9` : Ajusta el volumen del X9C103S del 0% al 90%.
* `s` : Alterna el modo *Standby* del sintonizador TEA5767.
* `u` : Realiza un *Seek $\uparrow$* en el TEA5767.

---

## 👥 Autores y Créditos

**Grupo F2 — Taller de Proyecto II (2026)**  
*Facultad de Informática — Universidad Nacional de La Plata (UNLP)*

* **Javier Alberto del Blanco** (Legajo: 02275/2)
* **Armando Antonio Pereira** (Legajo: 02530/8)
* **Álvaro Andrés Fricke** (Legajo: 01882/6)

**Docente:** Prof. Fernando Tinetti
