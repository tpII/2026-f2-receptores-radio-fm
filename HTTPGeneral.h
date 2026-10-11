#ifndef HTTPGEN
#define HTTPGEN

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ---------------- Pin del multiplexor CD4052 ----------------
// D4 = LOW  -> canal X0 (audio de la TEA5767)
// D4 = HIGH -> canal X1 (audio de la HEX3653)
// B (pata 9) e INH (pata 6) van a GND
#define PIN_MUX_SEL 2 // D4

// ---------------- Radios seleccionables ----------------
enum RadioActiva { RADIO_TEA = 0, RADIO_HEX = 1 };

extern ESP8266WebServer server;

// ---------------- Multiplexor ----------------
void mux_init();
void select_radio(RadioActiva radio);

// ---------------- Handlers ----------------
void handleRoot();
void handleMux();

// ---------------- Funciones de inicialización ----------------
void HTTP_init();

#endif
