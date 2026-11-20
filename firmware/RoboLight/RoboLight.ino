// ===== RoboLight - ESP32 controlado por página web (WebSocket) =====
#include <WiFi.h>              // Wi-Fi (aqui usado para criar a rede)
#include <WebServer.h>         // servidor web simples (porta 80)
#include <WebSocketsServer.h>  // comunicação em tempo real com a página (porta 81)

#define LED 2                  // LED da placa: acompanha o estado das lâmpadas UVC
#define UVC 32                 // pino que aciona as lâmpadas UVC (relé)
const int UVC_NIVEL_LIGADO = LOW;   // use LOW se o seu relé liga com nível baixo

WebServer server(80);                  // servidor que entrega a página HTML
WebSocketsServer webSocket(81);        // servidor que recebe os comandos dos botões
bool uvcLigado = false;                // guarda se as lâmpadas UVC estão ligadas

// Os arquivos abaixo precisam ficar na mesma pasta do .ino
// (a ordem importa: um usa o que o anterior criou)
#include "html_page.h"         // texto da página web
#include "PonteH.h"            // funções que mexem nos motores
#include "LCD.h"               // funções do display OLED
#include "WiFiAP.h"            // cria a rede Wi-Fi da ESP32
#include "functions.h"         // trata os comandos recebidos da página

void setup() {
  Serial.begin(115200);                  // liga o monitor serial para mensagens de teste
  pinMode(LED, OUTPUT);
  pinMode(UVC, OUTPUT);
  ligaUVC(false);                        // garante as lâmpadas desligadas ao ligar o robô

  Init_PonteH();                         // configura os pinos PWM das pontes H

  logoMekhos();                          // mostra o logo no display (se houver)
  Init_wifi();                           // cria a rede Wi-Fi
  LCDprintIP(AP_SSID, AP_SENHA);         // mostra no display o nome, a senha e o IP da rede

  server.on("/", webpage);               // quando abrirem o IP, entrega a página
  server.begin();                        // inicia o servidor web
  webSocket.begin();                     // inicia o servidor WebSocket
  webSocket.onEvent(webSocketEvent);     // diz qual função trata as mensagens recebidas
  webSocket.enableHeartbeat(2000, 1000, 2); // testa o cliente a cada 2s; se não responder, dispara DISCONNECTED
}

void loop() {
  webSocket.loop();          // verifica se chegou comando novo da página
  server.handleClient();     // atende quem está abrindo a página
  atualizarAceleracao();     // atualiza a rampa de velocidade continuamente
}
