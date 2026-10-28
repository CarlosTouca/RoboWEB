#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "new_html_pages_gamepad.h"

// Wi-Fi
const char* ssid = "redeteste";
const char* senha = "teste1234";

// Pinos dos motores
#define ESQ_LPWM 27
#define ESQ_RPWM 26
#define DIR_LPWM 05
#define DIR_RPWM 17

// LED
#define LED 2

// PWM
#define FREQ 20000
#define RES 8

// Variáveis
int velocidade = 150;
char direcao = 'P';
bool led = false;

WebServer server(80);
WebSocketsServer webSocket(81);


// Controla um motor
void motor(int LPWM, int RPWM, int valor) {

  if (valor > 0) {
    ledcWrite(LPWM, 0);
    ledcWrite(RPWM, valor);
  }
  else if (valor < 0) {
    ledcWrite(RPWM, 0);
    ledcWrite(LPWM, -valor);
  }
  else {
    ledcWrite(LPWM, 0);
    ledcWrite(RPWM, 0);
  }
}


// Controla os dois motores
void controlarMotores() {

  int esq = 0;
  int dir = 0;
  int metade = velocidade / 2;

  switch (direcao) {

    case 'F': // Frente
      esq = velocidade;
      dir = velocidade;
      break;

    case 'T': // Trás
      esq = -velocidade;
      dir = -velocidade;
      break;

    case 'E': // Esquerda
      esq = -velocidade;
      dir = velocidade;
      break;

    case 'D': // Direita
      esq = velocidade;
      dir = -velocidade;
      break;

    case 'G': // Frente esquerda
      esq = metade;
      dir = velocidade;
      break;

    case 'I': // Frente direita
      esq = velocidade;
      dir = metade;
      break;

    case 'H': // Trás esquerda
      esq = -metade;
      dir = -velocidade;
      break;

    case 'J': // Trás direita
      esq = -velocidade;
      dir = -metade;
      break;

    case 'P': // Parar
      esq = 0;
      dir = 0;
      break;
  }

  motor(ESQ_LPWM, ESQ_RPWM, esq);
  motor(DIR_LPWM, DIR_RPWM, dir);
}


// Recebe comandos da página
void receberMensagem(String msg) {

  if (msg.startsWith("ComandoDirecao=")) {
    direcao = msg.charAt(15);
    controlarMotores();
  }

  else if (msg.startsWith("Velocidade=")) {
    velocidade = map(msg.substring(11).toInt(), 0, 255, 0, 255);
    controlarMotores();
  }

  else if (msg.startsWith("LEDonoff=")) {

    led = msg.substring(9) == "ON";

    digitalWrite(LED, led);

    String resposta = "{\"LEDonoff\":\"";
    resposta += led ? "ON" : "OFF";
    resposta += "\"}";

    webSocket.broadcastTXT(resposta);
  }
}


// Eventos do WebSocket
void eventoWebSocket(uint8_t num, WStype_t tipo, uint8_t* payload, size_t tamanho) {

  if (tipo == WStype_TEXT) {
    receberMensagem(String((char*)payload));
  }

  else if (tipo == WStype_DISCONNECTED) {
    direcao = 'P';
    controlarMotores();
  }
}


// Página principal
void paginaInicial() {
  server.send(200, "text/html", webpageCode);
}


void setup() {

  Serial.begin(115200);

  // LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // PWM dos motores
  ledcAttach(ESQ_LPWM, FREQ, RES);
  ledcAttach(ESQ_RPWM, FREQ, RES);
  ledcAttach(DIR_LPWM, FREQ, RES);
  ledcAttach(DIR_RPWM, FREQ, RES);

  controlarMotores();

  // Cria Wi-Fi
    WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, senha);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  Serial.println();
  Serial.print("Rede criada: ");
  Serial.println(ssid);
  Serial.print("Abra no navegador: http://");
  Serial.println(WiFi.softAPIP());   // normalmente 192.168.4.1

  server.on("/", paginaInicial);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(eventoWebSocket);
}


void loop() {

  server.handleClient();
  webSocket.loop();
}