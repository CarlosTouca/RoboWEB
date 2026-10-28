// ===== RoboLight - ESP32 controlado por página web (WebSocket) =====
#include <WiFi.h>              // Wi-Fi (aqui usado para criar a rede)
#include <WebServer.h>         // servidor web simples (porta 80)
#include <WebSocketsServer.h>  // comunicação em tempo real com a página (porta 81)

#define LED 2                  // pino do LED da placa, controlado pela página
#define LED2 32                // pino da luz auxiliar, controlada pelo mesmo botão

WebServer server(80);                  // servidor que entrega a página HTML
WebSocketsServer webSocket(81);        // servidor que recebe os comandos dos botões
bool LEDonoff = false;                 // guarda se o LED está ligado (true) ou desligado (false)

// Os arquivos abaixo precisam ficar na mesma pasta do .ino
// (a ordem importa: um usa o que o anterior criou)
#include "html_page.h"         // texto da página web
#include "PonteH.h"            // funções que mexem nos motores
#include "LCD.h"               // funções do display OLED
#include "WiFiAP.h"            // cria a rede Wi-Fi da ESP32
#include "functions.h"         // trata os comandos recebidos da página

void setup() {
  Serial.begin(115200);                  // liga o monitor serial para mensagens de teste
  pinMode(LED, OUTPUT);                  // LED é saída
  pinMode(LED2, OUTPUT);                 // luz auxiliar é saída

  Init_PonteH();                         // configura os pinos PWM das pontes H
  logoMekhos();                          // mostra o logo no display
  Init_wifi();                           // cria a rede Wi-Fi
  LCDprintIP(AP_SSID, AP_SENHA);         // mostra no display o nome, a senha e o IP da rede

  server.on("/", webpage);               // quando abrirem o IP, entrega a página
  server.begin();                        // inicia o servidor web
  webSocket.begin();                     // inicia o servidor WebSocket
  webSocket.onEvent(webSocketEvent);     // diz qual função trata as mensagens recebidas
}

void loop() {
  webSocket.loop();      // verifica se chegou comando novo da página
  server.handleClient(); // atende quem está abrindo a página
}
