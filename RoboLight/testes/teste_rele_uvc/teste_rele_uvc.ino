#include <WiFi.h>
#include <WebServer.h>

#define RELAY_PIN 32  // Pino de controle do Módulo Relé (UV-C)
#define LED_STATUS 2  // LED integrado da ESP32

const char* ssid = "RoboLight-UVC-Test";
const char* password = "robolight123";

WebServer server(80);
bool relayEstado = false;

const char webpageHTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>RoboLight - Teste UV-C</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: sans-serif; }
    body { background-color: #0f172a; color: #f8fafc; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; padding: 20px; text-align: center; }
    .card { background: #1e293b; padding: 30px; border-radius: 16px; border: 1px solid #334155; width: 100%; max-width: 400px; }
    h1 { font-size: 1.4rem; color: #38bdf8; margin-bottom: 10px; }
    .status-box { background: #0f172a; padding: 15px; border-radius: 10px; margin: 20px 0; border: 1px solid #334155; }
    .status-value { font-size: 1.2rem; font-weight: bold; color: #ef4444; }
    .status-value.ativo { color: #10b981; }
    .btn-toggle { width: 100%; padding: 18px; font-size: 1.1rem; font-weight: bold; border: none; border-radius: 12px; cursor: pointer; color: #fff; background: #dc2626; }
    .btn-toggle.ativo { background: #10b981; }
  </style>
</head>
<body>
  <div class="card">
    <h1>RoboLight UV-C</h1>
    <p>Painel de Teste do Módulo Relé</p>
    <div class="status-box">
      <div id="txt-status" class="status-value">LÂMPADAS DESLIGADAS</div>
    </div>
    <button id="btn-relay" class="btn-toggle" onclick="alternarRelay()">LIGAR UV-C</button>
  </div>
  <script>
    function alternarRelay() {
      fetch('/toggle').then(r => r.json()).then(data => actualizar(data.estado));
    }
    function actualizar(estado) {
      const txt = document.getElementById('txt-status');
      const btn = document.getElementById('btn-relay');
      if (estado === "ON") {
        txt.innerText = "LÂMPADAS ENERGIZADAS (ON)"; txt.classList.add('ativo');
        btn.innerText = "DESLIGAR UV-C"; btn.classList.add('ativo');
      } else {
        txt.innerText = "LÂMPADAS DESLIGADAS (OFF)"; txt.classList.remove('ativo');
        btn.innerText = "LIGAR UV-C"; btn.classList.remove('ativo');
      }
    }
  </script>
</body>
</html>
)=====";

void handleRoot() { server.send(200, "text/html", webpageHTML); }
void handleToggle() {
  relayEstado = !relayEstado;
  digitalWrite(RELAY_PIN, relayEstado ? HIGH : LOW);
  digitalWrite(LED_STATUS, relayEstado ? HIGH : LOW);
  server.send(200, "application/json", "{\"estado\":\"" + String(relayEstado ? "ON" : "OFF") + "\"}");
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_STATUS, LOW);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();
}

void loop() { server.handleClient(); }
