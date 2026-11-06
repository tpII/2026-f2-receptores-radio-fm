#ifndef HANDLER_X9C103S_H
#define HANDLER_X9C103S_H

#include <Arduino.h>

// ---------------- Pines del X9C103S ----------------
#define POT_INC 14 // D5 - INC: cada flanco descendente mueve el cursor un paso
#define POT_UD  12 // D6 - U/D: HIGH = sube, LOW = baja
#define POT_CS  13 // D7 - CS: activo en LOW

#define POT_PASOS   99 // El X9C103S tiene 100 posiciones (0 a 99)
#define POT_INICIAL 50 // Volumen al arrancar (en %)

// La TEA5767 entrega más señal que la HEX3653: con el mismo % de la interfaz sonaba más fuerte.
// Cuando suena la TEA, la posición del cursor se escala por este factor (en %) para que
// el mismo volumen marcado se escuche igual en las dos radios. 100 = sin atenuación.
// Se calibra de oído con '+' / '-' por Monitor Serie y después se fija acá.
#define TEA_GANANCIA 50

// ---------------- Volumen y mute ----------------
void pot_set_volume(int porcentaje);
int  pot_get_volume();
void pot_toggle_mute();

// ---------------- Compensación entre radios ----------------
void pot_set_radio_tea(bool tea_activa); // Lo llama el relé al conmutar
void pot_set_ganancia_tea(int porcentaje);
int  pot_get_ganancia_tea();

// ---------------- Handlers ----------------
void handleVolumen();
void handleMute();

// ---------------- Funciones de inicialización ----------------
void potfunc_init();
void potHTTP_init();

#endif
