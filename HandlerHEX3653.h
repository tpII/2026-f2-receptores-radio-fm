#ifndef HANDLER_HEX3653_H
#define HANDLER_HEX3653_H

#include <Arduino.h>

// ---------------- Pines del HEX ----------------
// Cada pin emula un pulsador del módulo (reposo HIGH, pulsado LOW)
#define S_UP   15 // D8 - SEEK+
#define S_DOWN 0  // D3 - SEEK-
#define ON_OFF 2  // D4 - PSW (reposo HIGH, compatible con el arranque del ESP8266)


// ---------------- Envío de pulso a pin indicado ----------------
void pulso(int, unsigned int);

// ---------------- Handlers ----------------
void handleSUp();
void handleSDown();
void handleOnOff();

// ---------------- Funciones de inicialización ----------------
void hexpins_init();
void hexHTTP_init();

#endif
