#include "HandlerX9C103S.h"
#include "HTTPGeneral.h"

static int  posicion = 0;            // Posición actual del cursor (0 a POT_PASOS)
static int  volumen = POT_INICIAL;   // Volumen elegido (en %), se conserva durante el mute
static bool silenciado = false;

// ---------------- Movimiento del cursor ----------------

// Mueve el cursor 'pasos' posiciones hacia arriba o abajo
static void pot_mover(bool subir, int pasos) {
  if (pasos <= 0) return;

  digitalWrite(POT_UD, subir ? HIGH : LOW);
  delayMicroseconds(5);
  digitalWrite(POT_CS, LOW);
  delayMicroseconds(5);
  for (int i = 0; i < pasos; i++) {
    digitalWrite(POT_INC, LOW);
    delayMicroseconds(5);
    digitalWrite(POT_INC, HIGH);
    delayMicroseconds(5);
  }
  // Se sube CS con INC en LOW para que el chip NO grabe la posición en su memoria
  // no volátil (tiene escrituras limitadas y en cada arranque se recalibra desde 0)
  digitalWrite(POT_INC, LOW);
  delayMicroseconds(5);
  digitalWrite(POT_CS, HIGH);
  delayMicroseconds(5);
  digitalWrite(POT_INC, HIGH);
}

// Lleva el cursor a la posición correspondiente al porcentaje indicado
static void pot_aplicar(int porcentaje) {
  int destino = (porcentaje * POT_PASOS + 50) / 100;
  if (destino > posicion)      pot_mover(true, destino - posicion);
  else if (destino < posicion) pot_mover(false, posicion - destino);
  posicion = destino;
}

// ---------------- Volumen y mute ----------------

void pot_set_volume(int porcentaje) {
  volumen = constrain(porcentaje, 0, 100);
  silenciado = false; // Mover el volumen quita el mute
  pot_aplicar(volumen);
}

int pot_get_volume() { return volumen; }

// El mute baja el potenciómetro a 0; al quitarlo vuelve al volumen elegido
void pot_toggle_mute() {
  silenciado = !silenciado;
  pot_aplicar(silenciado ? 0 : volumen);
}

// Estado en JSON: { volume, muted }
static void enviarEstado() {
  String json = "{";
  json += "\"volume\": " + String(volumen) + ",";
  json += "\"muted\": " + String(silenciado ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

// ---------------- Handlers ----------------

// /volumen?v=0..100 -> fija el volumen; sin parámetro devuelve el estado
void handleVolumen() {
  if (server.hasArg("v")) pot_set_volume(server.arg("v").toInt());
  enviarEstado();
}

// /mute -> alterna el silencio (vale para ambas radios)
void handleMute() {
  pot_toggle_mute();
  enviarEstado();
}

// ---------------- Inicializaciones ----------------

void potfunc_init() {
  pinMode(POT_INC, OUTPUT);
  pinMode(POT_UD, OUTPUT);
  pinMode(POT_CS, OUTPUT);
  digitalWrite(POT_CS, HIGH);
  digitalWrite(POT_INC, HIGH);

  // No se conoce la posición guardada: se lleva el cursor al mínimo para calibrar
  pot_mover(false, POT_PASOS + 1);
  posicion = 0;
  pot_set_volume(POT_INICIAL);

  Serial.println("[X9C103S] Potenciómetro inicializado al 50%.");
}

void potHTTP_init()
{
  // Configurar rutas
  server.on("/volumen", handleVolumen);
  server.on("/mute", handleMute);
}
