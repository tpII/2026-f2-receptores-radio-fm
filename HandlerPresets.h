#ifndef HANDLER_PRESETS_H
#define HANDLER_PRESETS_H

#include <Arduino.h>

#define PRESETS_COUNT 5

// ---------------- Estructura en Memoria No Volátil (EEPROM) ----------------
#pragma pack(push, 1)
struct PresetStorage {
  uint32_t magic;                      // Identificador mágico 'PRST' (0x50525354)
  uint8_t  version;                    // Versión del formato (1)
  float    frequencies[PRESETS_COUNT]; // Frecuencias en MHz (ej: 88.1, 91.5, 95.5, 101.5, 107.9)
  uint8_t  checksum;                   // Checksum para validación de integridad
};
#pragma pack(pop)

// ---------------- Funciones de Gestión de Presets ----------------
void   presets_init();
bool   presets_save(uint8_t index, float frequency);
float  presets_get(uint8_t index);
String presets_get_json();

// ---------------- Handlers de Rutas HTTP ----------------
void handlePresetsGet();
void handlePresetsSave();
void handlePresetsSelect();

// ---------------- Inicialización de Rutas ----------------
void presetsHTTP_init();

#endif // HANDLER_PRESETS_H
