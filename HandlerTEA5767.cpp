#include "HandlerTEA5767.h"
#include "HTTPGeneral.h"

TEA5767 receptor;
tea5767_info_t estado;

// ---------------- Control del TEA ----------------

void tea_seek_up() {
  receptor.searchUp(); delay(80);
  receptor.getInfo(&estado);
}

void tea_seek_down() {
  receptor.searchDown(); delay(80);
  receptor.getInfo(&estado);
}

void tea_set_frequency(float mhz) {
  receptor.setFrequency(mhz); delay(80);
  receptor.getInfo(&estado);
}

// Al salir del standby el cristal de 32.768 kHz del TEA tarda en estabilizarse:
// si se sintoniza enseguida queda corrido (nivel de señal ~0). Por eso se vuelve
// a enviar la misma sintonía un rato después, desde tea_loop(), sin bloquear el servidor.
#define TEA_RESINTONIZAR_MS 1500
static unsigned long tea_desperto_ms = 0;
static bool tea_resintonizar = false;

void tea_toggle_standby() {
  if (estado.standby) {
    receptor.awake();
    tea_desperto_ms = millis();
    tea_resintonizar = true;
  } else {
    receptor.standby();
    tea_resintonizar = false;
  }
  receptor.getInfo(&estado);
}

bool tea_is_standby() { return estado.standby; }

void tea_loop() {
  if (tea_resintonizar && millis() - tea_desperto_ms >= TEA_RESINTONIZAR_MS) {
    tea_resintonizar = false;
    tea5767_retune(); // Misma sintonía exacta que antes del standby
    delay(80);
    receptor.getInfo(&estado);
  }
}

// Estado en JSON: { standby, frequency, quality }
String tea_get_status_json() {
  receptor.getInfo(&estado);
  String json = "{";
  json += "\"standby\": " + String(estado.standby ? "true" : "false") + ",";
  json += "\"frequency\": " + String(estado.mhz, 2) + ",";
  json += "\"quality\": " + String(estado.signal_level);
  json += "}";
  return json;
}

// ---------------- Handlers ----------------
// Cada comando responde con el estado actualizado del TEA

void handleUp()     { tea_seek_up();        server.send(200, "application/json", tea_get_status_json()); }
void handleDown()   { tea_seek_down();      server.send(200, "application/json", tea_get_status_json()); }
void handleSleep()  { tea_toggle_standby(); server.send(200, "application/json", tea_get_status_json()); }
void handleStatus() {                       server.send(200, "application/json", tea_get_status_json()); }

// /set_freq?v=101.9
void handleFreq() {
  if (!server.hasArg("v")) {
    server.send(400, "text/plain", "Falta parámetro ?v=");
    return;
  }
  String param = server.arg("v");
  param.replace(',', '.');
  float valor = param.toFloat();
  // toFloat() devuelve 0 si el texto no es un número, así que también queda descartado
  if (valor < TEA5767_US_EUROPE_BAND_MIN || valor > TEA5767_US_EUROPE_BAND_MAX) {
    server.send(400, "text/plain", "Frecuencia fuera de rango (87.5 a 108 MHz)");
    return;
  }
  tea_set_frequency(valor);
  server.send(200, "application/json", tea_get_status_json());
}

// ---------------- Inicializaciones ----------------

void teafunc_init(){
  // Inicializar el bus I2C y el receptor
  Wire.begin(TEA_SDA, TEA_SCL);
  Wire.setClock(100000);

  receptor = TEA5767();

  receptor.unmute();
  receptor.awake();
  receptor.setFrequency(92.1); delay(80);

  receptor.getInfo(&estado);
  Serial.println("[TEA5767] Sintonizador inicializado en 92.10 MHz.");
}

void teaHTTP_init()
{
  // Configurar rutas
  server.on("/_up", handleUp);
  server.on("/_down", handleDown);
  server.on("/set_freq", HTTP_GET, handleFreq);
  server.on("/stand_by", handleSleep);
  server.on("/tea/status", handleStatus);
}
