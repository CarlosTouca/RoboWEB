// ===== Funções do servidor =====

// Envia a página HTML para quem abriu o IP no navegador
void webpage() {
  server.send_P(200, "text/html", webpageCode);   // envia direto da memória flash, sem copiar a página para a RAM
}

// Liga ou desliga as lâmpadas UVC e avisa todas as páginas abertas
void ligaUVC(bool ligar) {
  digitalWrite(UVC, ligar ? UVC_NIVEL_LIGADO : !UVC_NIVEL_LIGADO);  // sempre reaplica o pino (garante desligado)
  digitalWrite(LED, ligar);                                         // LED da placa acompanha
  if (ligar == uvcLigado) return;                                   // estado não mudou: não avisa as páginas de novo
  uvcLigado = ligar;
  webSocket.broadcastTXT(ligar ? "UVON" : "UVOFF");
}

// Chamada automaticamente a cada evento do WebSocket
// num = número do cliente, type = tipo do evento, payload = texto recebido
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t tamanho) {

  // Cliente acabou de conectar: manda o estado atual das UVC e da velocidade para a página
  if (type == WStype_CONNECTED) {
    webSocket.sendTXT(num, uvcLigado ? "UVON" : "UVOFF");
    char buf[8];
    sprintf(buf, "V%d", velPct);       // monta o texto, ex: "V80"
    webSocket.sendTXT(num, buf);
    return;
  }

  // Cliente saiu (fechou a página ou perdeu o Wi-Fi): para tudo na hora por segurança
  if (type == WStype_DISCONNECTED) {
    paradaImediata();
    ligaUVC(false);
    return;
  }

  if (type != WStype_TEXT) return;   // ignora qualquer outro tipo de evento

  char *msg = (char *)payload;       // texto recebido, ex: "F", "UVON", "V80"

  // Lâmpadas UVC
  if (strcmp(msg, "UVON") == 0)  { ligaUVC(true);  return; }
  if (strcmp(msg, "UVOFF") == 0) { ligaUVC(false); return; }

  // Velocidade: "V80" = 80%. atoi(msg + 1) lê o número depois do "V"
  if (msg[0] == 'V') {
    velPct = constrain(atoi(msg + 1), 0, 100);   // garante que fica entre 0 e 100
    atualizarAlvos();                            // aplica a nova velocidade na hora
    return;
  }

  // Comandos de direção: uma letra só
  switch (msg[0]) {
    case 'F': frente();   break;  // Frente
    case 'T': tras();     break;  // Trás
    case 'D': direita();  break;  // Direita
    case 'E': esquerda(); break;  // Esquerda
    case 'P': parado();   break;  // Parado com rampa (a página manda ao soltar o botão)
    case 'X': paradaImediata(); ligaUVC(false); break;  // Emergência: para sem rampa e apaga as UVC
  }
}
