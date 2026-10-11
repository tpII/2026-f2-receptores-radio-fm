#ifndef HANDLER_X9C103S_H
#define HANDLER_X9C103S_H

#include <Arduino.h>

// ---------------- Pines del X9C103S ----------------
#define POT_INC 14 // D5 - INC: cada flanco descendente mueve el cursor un paso
#define POT_UD  12 // D6 - U/D: HIGH = sube, LOW = baja
#define POT_CS  13 // D7 - CS: activo en LOW

#define POT_PASOS   99 // El X9C103S tiene 100 posiciones (0 a 99)
#define POT_INICIAL 50 // Volumen al arrancar (en %)

// ---------------- Volumen y mute ----------------
void pot_set_volume(int porcentaje);
int  pot_get_volume();
void pot_toggle_mute();

// ---------------- Handlers ----------------
void handleVolumen();
void handleMute();

// ---------------- Funciones de inicialización ----------------
void potfunc_init();
void potHTTP_init();

#endif
