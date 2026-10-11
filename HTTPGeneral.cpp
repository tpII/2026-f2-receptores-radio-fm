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


// ---------------- Multiplexor CD4052 ----------------

static RadioActiva radio_activa = RADIO_TEA; // Al arrancar suena la TEA5767

void mux_init() {
  pinMode(PIN_MUX_SEL, OUTPUT);
  select_radio(RADIO_TEA);
}

// Conmuta la salida de audio a la radio indicada
void select_radio(RadioActiva radio) {
  radio_activa = radio;
  digitalWrite(PIN_MUX_SEL, radio == RADIO_HEX ? HIGH : LOW);
  Serial.println(radio == RADIO_HEX ? "[CD4052] Canal conmutado a HEX3653 (D4 = HIGH)."
                                   : "[CD4052] Canal conmutado a TEA5767 (D4 = LOW).");
}


// ---------------- Handlers ----------------

void handleRoot() { server.send(200, "text/html", index_html); }

// /mux?radio=tea|hex -> conmuta el CD4052; siempre responde la radio activa ("tea" o "hex")
void handleMux() {
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
  server.on("/mux", handleMux);

  hexHTTP_init(); // Rutas del HEX
  teaHTTP_init(); // Rutas del TEA
  potHTTP_init(); // Rutas del volumen

  server.begin();
  Serial.println("[HTTP] Servidor HTTP iniciado");
}
