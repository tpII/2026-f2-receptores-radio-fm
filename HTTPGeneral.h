#ifndef HTTPGEN
#define HTTPGEN

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ---------------- Pin del relé (módulo HL-51, Songle SRD-05VDC-SL-C) ----------------
// El módulo se activa por nivel BAJO y para soltarse necesita ~5V en IN (el ESP8266 solo da 3,3V):
// D0 como entrada (suelto) -> el módulo sube IN a 5V, relé apagado, COM unido a NC (audio de la HEX3653)
// D0 como salida en LOW    -> relé activado, COM unido a NO (audio de la TEA5767)
// Con D0 suelto el pin queda expuesto a ~5V, por encima de lo que garantiza el ESP8266.
// VCC del módulo a VIN (5V), GND a GND, IN a D0.
// No se usa D4 (GPIO2): la entrada del módulo lo tira a LOW y el ESP8266 no arranca.
#define PIN_RELE 16 // D0

// ---------------- Radios seleccionables ----------------
enum RadioActiva { RADIO_TEA = 0, RADIO_HEX = 1 };

extern ESP8266WebServer server;

// ---------------- Relé ----------------
void rele_init();
void select_radio(RadioActiva radio);

// ---------------- Handlers ----------------
void handleRoot();
void handleRadio();

// ---------------- Funciones de inicialización ----------------
void HTTP_init();

#endif
