// ===== Funções do servidor =====

// Envia a página HTML para quem abriu o IP no navegador
void webpage() {
  server.send(200, "text/html", webpageCode);
}

// Chamada automaticamente a cada evento do WebSocket
// num = número do cliente, type = tipo do evento, payload = texto recebido
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t tamanho) {

  // Cliente acabou de conectar: manda o estado atual do LED e da velocidade para a página
  if (type == WStype_CONNECTED) {
    webSocket.sendTXT(num, LEDonoff ? "ON" : "OFF");
    char buf[8];
    sprintf(buf, "V%d", velPct);       // monta o texto, ex: "V80"
    webSocket.sendTXT(num, buf);
    return;
  }

  // Cliente saiu (fechou a página ou perdeu o Wi-Fi): para o robô por segurança
  if (type == WStype_DISCONNECTED) {
    parado();
    return;
  }

  if (type != WStype_TEXT) return;   // ignora qualquer outro tipo de evento

  char *msg = (char *)payload;       // texto recebido, ex: "F", "ON", "V80"
  Serial.println(msg);               // mostra no monitor serial (útil para testar)

  // Comandos do LED: "ON" liga, "OFF" desliga
  if (strcmp(msg, "ON") == 0 || strcmp(msg, "OFF") == 0) {
    LEDonoff = (msg[1] == 'N');          // "ON" tem 'N' na 2ª letra, "OFF" não
    digitalWrite(LED, LEDonoff);         // liga ou desliga o LED da placa
    digitalWrite(LED2, LEDonoff);        // liga ou desliga a luz auxiliar
    webSocket.broadcastTXT(LEDonoff ? "ON" : "OFF"); // avisa todas as páginas abertas
    return;
  }

  // Velocidade: "V80" = 80%. atoi(msg + 1) lê o número depois do "V"
  if (msg[0] == 'V') {
    velPct = constrain(atoi(msg + 1), 0, 100);   // garante que fica entre 0 e 100
    return;                                      // vale a partir da próxima direção apertada
  }

  // Comandos de direção: uma letra só
  switch (msg[0]) {
    case 'F': frente();   break;  // Frente
    case 'T': tras();     break;  // Trás
    case 'D': direita();  break;  // Direita
    case 'E': esquerda(); break;  // Esquerda
    case 'P': parado();   break;  // Parado (a página manda ao soltar o botão)
  }
}
