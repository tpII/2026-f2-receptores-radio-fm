#include "HandlerPresets.h"
#include "../servidor/HTTPGeneral.h"
#include "HandlerTEA5767.h"
#include <EEPROM.h>

#define PRESETS_MAGIC 0x50525354 // 'PRST'
#define PRESETS_EEPROM_ADDR 0

static const float PRESETS_DEFAULTS[PRESETS_COUNT] = {
  88.10f, 91.50f, 95.50f, 101.50f, 107.90f
};

static PresetStorage presets_mem;

// ---------------- Checksum para integridad de datos ----------------
static uint8_t calcular_checksum(const PresetStorage& data) {
  uint8_t sum = 0xAA;
  const uint8_t* p = (const uint8_t*)data.frequencies;
  for (size_t i = 0; i < sizeof(data.frequencies); i++) {
    sum ^= p[i];
    sum = (sum << 1) | (sum >> 7);
  }
  sum ^= data.version;
  return sum;
}

// ---------------- Funciones de Gestión de Presets ----------------

void presets_init() {
  EEPROM.begin(64);
  PresetStorage temp;
  EEPROM.get(PRESETS_EEPROM_ADDR, temp);

  bool valido = (temp.magic == PRESETS_MAGIC) &&
                (temp.version == 1) &&
                (temp.checksum == calcular_checksum(temp));

  // Validar también que todas las frecuencias pertenezcan a la banda FM (87.5 a 108.0 MHz)
  if (valido) {
    for (uint8_t i = 0; i < PRESETS_COUNT; i++) {
      if (isnan(temp.frequencies[i]) || temp.frequencies[i] < 87.5f || temp.frequencies[i] > 108.0f) {
        valido = false;
        break;
      }
    }
  }

  if (!valido) {
    Serial.println("[PRESETS] Memoria EEPROM vacia o no valida. Inicializando valores por defecto...");
    presets_mem.magic = PRESETS_MAGIC;
    presets_mem.version = 1;
    for (uint8_t i = 0; i < PRESETS_COUNT; i++) {
      presets_mem.frequencies[i] = PRESETS_DEFAULTS[i];
    }
    presets_mem.checksum = calcular_checksum(presets_mem);

    EEPROM.put(PRESETS_EEPROM_ADDR, presets_mem);
    EEPROM.commit();
    Serial.println("[PRESETS] Frecuencias por defecto guardadas en EEPROM.");
  } else {
    presets_mem = temp;
    Serial.println("[PRESETS] Presintonias cargadas exitosamente desde EEPROM.");
  }

  for (uint8_t i = 0; i < PRESETS_COUNT; i++) {
    Serial.printf("[PRESETS] Slot %d (P%d): %.2f MHz\n", i, i + 1, presets_mem.frequencies[i]);
  }
}

bool presets_save(uint8_t index, float frequency) {
  if (index >= PRESETS_COUNT) {
    Serial.printf("[PRESETS] Error: indice fuera de rango (%d)\n", index);
    return false;
  }
  if (isnan(frequency) || frequency < 87.5f || frequency > 108.0f) {
    Serial.printf("[PRESETS] Error: frecuencia invalida (%.2f MHz)\n", frequency);
    return false;
  }

  presets_mem.frequencies[index] = frequency;
  presets_mem.checksum = calcular_checksum(presets_mem);

  EEPROM.put(PRESETS_EEPROM_ADDR, presets_mem);
  bool ok = EEPROM.commit();
  if (ok) {
    Serial.printf("[PRESETS] Guardado Slot %d (P%d): %.2f MHz en EEPROM.\n", index, index + 1, frequency);
  } else {
    Serial.printf("[PRESETS] Error al persistir Slot %d en EEPROM.\n", index);
  }
  return ok;
}

float presets_get(uint8_t index) {
  if (index >= PRESETS_COUNT) {
    return 0.0f;
  }
  return presets_mem.frequencies[index];
}

String presets_get_json() {
  String json = "{\"presets\":[";
  for (uint8_t i = 0; i < PRESETS_COUNT; i++) {
    if (i > 0) json += ",";
    json += "{\"slot\":";
    json += String(i);
    json += ",\"frequency\":";
    json += String(presets_mem.frequencies[i], 2);
    json += "}";
  }
  json += "]}";
  return json;
}

// ---------------- Handlers de Rutas HTTP ----------------

// GET /api/presets -> devuelve lista JSON de los 5 presets guardados
void handlePresetsGet() {
  server.send(200, "application/json", presets_get_json());
}

// Lee el parametro slot (o index), tiene que ser un numero de 0 a 4 (0 = P1).
// Devuelve -1 si esta mal. Revisamos que sean digitos porque toInt() devuelve 0 si le pasas texto
static int leer_slot() {
  String param = server.hasArg("slot") ? server.arg("slot") : server.arg("index");
  param.trim();
  if (param.length() == 0) return -1;
  for (unsigned int i = 0; i < param.length(); i++) {
    if (!isDigit(param[i])) return -1;
  }
  int slot = param.toInt();
  return (slot < PRESETS_COUNT) ? slot : -1;
}

// POST /api/presets o GET /api/presets/save?slot=X&freq=Y
void handlePresetsSave() {
  int slot = leer_slot();
  if (slot < 0) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Parametro slot invalido (debe ser 0 a 4)\"}");
    return;
  }

  float freq = 0.0f;
  if (server.hasArg("freq")) {
    String param = server.arg("freq");
    param.replace(',', '.');
    freq = param.toFloat();
  } else if (server.hasArg("v")) {
    String param = server.arg("v");
    param.replace(',', '.');
    freq = param.toFloat();
  } else if (server.hasArg("frequency")) {
    String param = server.arg("frequency");
    param.replace(',', '.');
    freq = param.toFloat();
  } else {
    // Si no se especifica frecuencia, se utiliza la sintonizada actualmente en el TEA5767
    freq = tea_get_frequency();
  }

  if (isnan(freq) || freq < 87.5f || freq > 108.0f) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Frecuencia fuera de rango (87.5 a 108.0 MHz)\"}");
    return;
  }

  bool ok = presets_save((uint8_t)slot, freq);
  if (!ok) {
    server.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Fallo al escribir en EEPROM\"}");
    return;
  }

  // Le sacamos la llave del principio al json de presets para que quede todo en el mismo objeto
  String resp = "{\"status\":\"ok\",\"slot\":" + String(slot) + ",\"frequency\":" + String(freq, 2) + "," + presets_get_json().substring(1);
  server.send(200, "application/json", resp);
}

// GET /api/presets/select?slot=X -> sintoniza el preset X
void handlePresetsSelect() {
  if (!server.hasArg("slot") && !server.hasArg("index")) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Falta parametro ?slot=\"}");
    return;
  }

  int slot = leer_slot();
  if (slot < 0) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Slot fuera de rango (0 a 4)\"}");
    return;
  }

  float f = presets_get((uint8_t)slot);

  // Pasamos el relé a la TEA y sintonizamos
  select_radio(RADIO_TEA);
  tea_set_frequency(f);

  String teaJson = tea_get_status_json();
  String resp = "{\"status\":\"ok\",\"slot\":" + String(slot) + "," + teaJson.substring(1);
  server.send(200, "application/json", resp);
}

// ---------------- Inicialización de Rutas HTTP ----------------

void presetsHTTP_init() {
  server.on("/api/presets", HTTP_GET, handlePresetsGet);
  server.on("/api/presets", HTTP_POST, handlePresetsSave);
  server.on("/api/presets/save", HTTP_GET, handlePresetsSave);
  server.on("/api/presets/save", HTTP_POST, handlePresetsSave);
  server.on("/api/presets/select", HTTP_GET, handlePresetsSelect);
}
