#include "HTTPGeneral.h"
#include "InterfazWeb.h"
#include "HandlerTEA5767.h"
#include "HandlerHEX3653.h"
#include "HandlerX9C103S.h"

// ---------------- Claves de la conexión WiFi ----------------
const char* ssid = "TP2F2_AP";       // Nombre de la red que va a crear
const char* password = "12345678";   // Contraseña mínima 8 caracteres


// ---------------- Configuración de IPs ----------------
ESP8266WebServer server(80);
// Configuración IP fija para el AP
IPAddress local_IP(10, 10, 10, 10);
IPAddress gateway(10, 10, 10, 10);      // Puede ser igual a la IP local
IPAddress subnet(255, 255, 255, 0);


// ---------------- Relé selector de audio ----------------

static RadioActiva radio_activa = RADIO_HEX; // Al arrancar suena la HEX3653 (relé apagado)

void rele_init() {
  select_radio(RADIO_HEX);
}

// Conmuta la salida de audio a la radio indicada
void select_radio(RadioActiva radio) {
  radio_activa = radio;
  if (radio == RADIO_TEA) {
    pinMode(PIN_RELE, OUTPUT);
    digitalWrite(PIN_RELE, LOW); // Activa el relé
  } else {
    pinMode(PIN_RELE, INPUT);    // Suelta el pin: el módulo sube IN y el relé se apaga
  }
  Serial.println(radio == RADIO_TEA ? "[RELE] Audio conmutado a TEA5767 (D0 = LOW, rele activado, COM-NO)."
                                   : "[RELE] Audio conmutado a HEX3653 (D0 suelto, rele apagado, COM-NC).");
}


// ---------------- Handlers ----------------

void handleRoot() { server.send(200, "text/html", index_html); }

// /radio?radio=tea|hex -> conmuta el relé; siempre responde la radio activa ("tea" o "hex")
void handleRadio() {
  if (server.hasArg("radio")) {
    String r = server.arg("radio");
    if (r == "tea")      select_radio(RADIO_TEA);
    else if (r == "hex") select_radio(RADIO_HEX);
  }
  server.send(200, "text/plain", radio_activa == RADIO_TEA ? "tea" : "hex");
}


// ---------------- Levantamiento de la página ----------------

void HTTP_init()
{
  // Crear Access Point con IP fija
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.print("[WiFi] Punto de acceso iniciado: ");
  Serial.println(ssid);
  Serial.print("[WiFi] IP del AP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/radio", handleRadio);

  hexHTTP_init(); // Rutas del HEX
  teaHTTP_init(); // Rutas del TEA
  potHTTP_init(); // Rutas del volumen

  server.begin();
  Serial.println("[HTTP] Servidor HTTP iniciado");
}
