#ifndef HTTPGEN
#define HTTPGEN

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ---------------- Pin del relé (módulo HL-51, Songle SRD-05VDC-SL-C) ----------------
// El modulo se activa con LOW y para apagarse necesita 5V en IN (el ESP8266 da 3,3V).
// D0 como entrada    -> el modulo sube IN a 5V, relé apagado, COM con NC (audio de la HEX3653)
// D0 como salida LOW -> relé prendido, COM con NO (audio de la TEA5767)
// Ojo: con D0 como entrada el pin queda en ~5V, que es mas de lo que aguanta el ESP8266.
// VCC del modulo a VIN (5V), GND a GND, IN a D0.
// No usamos D4 (GPIO2) porque el modulo lo tira a LOW y el ESP8266 no arranca.
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
