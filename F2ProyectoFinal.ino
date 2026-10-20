#include "HTTPGeneral.h"
#include "HandlerHEX3653.h"
#include "HandlerTEA5767.h"
#include "HandlerX9C103S.h"

void setup() {
  Serial.begin(115200);

  rele_init();    // Relé HL-51 (D0), arranca apagado con la HEX
  potfunc_init(); // X9C103S (D5, D6, D7), calibra y deja el volumen al 50%
  hexpins_init(); // HEX3653 (D4, D3, D8)
  teafunc_init(); // TEA5767 por I2C (D1, D2)

  HTTP_init();
}

// ---------------- Comandos de prueba por Monitor Serie ----------------
// 't' / 'h'  -> conmuta el relé a la TEA / HEX
// '0'..'9'   -> volumen 0% .. 90%
// 's'        -> alterna el standby del TEA
// 'u'        -> Seek ↑ del TEA
// 'i'        -> imprime el estado del TEA
// 'r'        -> imprime los 5 bytes crudos que devuelve el TEA (I2C 0x60)
void comandosSerie() {
  if (!Serial.available()) return;
  char c = Serial.read();

  if (c == 't') select_radio(RADIO_TEA);
  else if (c == 'h') select_radio(RADIO_HEX);
  else if (c >= '0' && c <= '9') {
    pot_set_volume((c - '0') * 10);
    Serial.printf("[X9C103S] Volumen fijado en %d%%.\n", pot_get_volume());
  }
  else if (c == 's') {
    tea_toggle_standby();
    Serial.println(tea_is_standby() ? "[TEA5767] Standby ACTIVADO" : "[TEA5767] Standby DESACTIVADO");
  }
  else if (c == 'u') {
    tea_seek_up();
    Serial.println("[TEA5767] Seek UP terminado");
  }
  else if (c == 'i') Serial.println("[TEA5767] " + tea_get_status_json());
  else if (c == 'r') {
    Wire.requestFrom(0x60, 5);
    Serial.print("[TEA5767] RAW:");
    while (Wire.available()) Serial.printf(" %02X", Wire.read());
    Serial.println();
  }
}

void loop() {
  server.handleClient();
  tea_loop();       // Resintoniza el TEA después de salir del standby
  comandosSerie();
}
