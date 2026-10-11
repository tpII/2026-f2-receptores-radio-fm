#include "HandlerHEX3653.h"
#include "HTTPGeneral.h"

// ---------------- Función para enviar pulso ----------------

void pulso(int pin, unsigned int t) {
  digitalWrite(pin, LOW);
  delay(t);
  digitalWrite(pin, HIGH);
}

// ---------------- Handlers ----------------
void handleSUp()    { pulso(S_UP,120); server.send(200, "text/plain", "S_UP pulso enviado"); }
void handleSDown()  { pulso(S_DOWN,120); server.send(200, "text/plain", "S_DOWN pulso enviado"); }
void handleOnOff()  { pulso(ON_OFF,120); server.send(200, "text/plain", "ON_OFF pulso enviado"); }

// ---------------- Inicializaciones ----------------

void hexpins_init(){
  // Configurar pines, en reposo HIGH
  pinMode(S_UP, OUTPUT);
  pinMode(S_DOWN, OUTPUT);
  pinMode(ON_OFF, OUTPUT);

  digitalWrite(S_UP, HIGH);
  digitalWrite(S_DOWN, HIGH);
  digitalWrite(ON_OFF, HIGH);

  Serial.println("[HEX3653] Pines configurados (D0:PSW, D3:SEEK-, D8:SEEK+).");
}

void hexHTTP_init()
{
  // Configurar rutas
  server.on("/s_up", handleSUp);
  server.on("/s_down", handleSDown);
  server.on("/on_off", handleOnOff);
}
