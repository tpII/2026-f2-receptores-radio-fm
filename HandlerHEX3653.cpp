#include "HandlerHEX3653.h"
#include "HTTPGeneral.h"
#include <user_interface.h> // system_get_rst_info()

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

  Serial.println("[HEX3653] Pines configurados (D4:PSW, D3:SEEK-, D8:SEEK+).");
}

// PSW es un pulsador que CONMUTA (encendido <-> apagado) y la HEX no avisa su estado.
// Si se pulsara en cada arranque, un reset del ESP sin cortar la energía (botón RST,
// subida de firmware, watchdog) la APAGARÍA. Por eso solo se pulsa cuando el ESP
// arrancó por corte de energía: ahí la HEX, que comparte la alimentación, también
// arrancó de cero y apagada.
void hex_encender_al_inicio() {
#if HEX_ENCENDER_AL_INICIO
  uint32_t motivo = system_get_rst_info()->reason;
  if (motivo != REASON_DEFAULT_RST) {
    Serial.printf("[HEX3653] Reinicio sin corte de energía (motivo %u): no se pulsa PSW.\n", motivo);
    return;
  }
  while (millis() < HEX_T_ARRANQUE) delay(10); // Espera a que la HEX termine de arrancar
  pulso(ON_OFF, HEX_T_PULSO_PSW);
  Serial.println("[HEX3653] Pulso de encendido enviado (PSW).");
#endif
}

void hexHTTP_init()
{
  // Configurar rutas
  server.on("/s_up", handleSUp);
  server.on("/s_down", handleSDown);
  server.on("/on_off", handleOnOff);
}
