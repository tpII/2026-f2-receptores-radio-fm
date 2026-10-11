#ifndef HANDLER_TEA5767_H
#define HANDLER_TEA5767_H

#include <Arduino.h>
#include <Wire.h>
#include "TEA5767.h"

// ---------------- Pines del TEA (I2C) ----------------
#define TEA_SCL 5 // D1
#define TEA_SDA 4 // D2

// ---------------- Control del TEA ----------------
void tea_seek_up();
void tea_seek_down();
void tea_set_frequency(float mhz);
void tea_toggle_standby();
bool tea_is_standby();
void tea_loop(); // Llamar en loop(): resintoniza tras salir del standby
String tea_get_status_json();

// ---------------- Handlers ----------------
void handleUp();
void handleDown();
void handleFreq();
void handleSleep();
void handleStatus();

// ---------------- Funciones de inicialización ----------------
void teafunc_init();
void teaHTTP_init();

#endif
