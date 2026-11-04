// ===== Controle dos motores com duas pontes H (BTS7960) e Aceleração Suave =====
const int FREQ = 5000;   // frequência do PWM em Hz
const int RES  = 8;      // resolução de 8 bits (valores de 0 a 255)
int velPct = 100;        // velocidade em % (0 a 100), muda pela barra da página

// Pinos da ponte 1 (motor esquerdo) e da ponte 2 (motor direito)
const int RPWM1 = 18;  // ponte 1, sentido 1
const int LPWM1 = 19;  // ponte 1, sentido 2
const int RPWM2 = 5;   // ponte 2, sentido 1
const int LPWM2 = 17;  // ponte 2, sentido 2

// --- Variáveis de aceleração ---
int targetA = 0, targetB = 0, targetC = 0, targetD = 0; // Alvos calculados (0 a 255)
float currentA = 0, currentB = 0, currentC = 0, currentD = 0; // Velocidade atual dos motores
char sentidoAtual = 'P'; // Guarda a direção atual ('F', 'T', 'E', 'D', 'P')

unsigned long tempoAnteriorAcel = 0;
const int INTERVALO_ACEL = 10; // Intervalo de atualização da rampa (ms)
const float PASSO_ACEL   = 4.0; // Incremento de PWM por passo (aumente para acelerar mais rápido)

// Prepara os 4 canais de PWM e liga cada um a um pino
void Init_PonteH() {
  for (int c = 0; c < 4; c++) ledcSetup(c, FREQ, RES); // configura os canais 0, 1, 2 e 3
  ledcAttachPin(RPWM1, 0);  // canal 0 -> pino RPWM1
  ledcAttachPin(LPWM1, 1);  // canal 1 -> pino LPWM1
  ledcAttachPin(RPWM2, 2);  // canal 2 -> pino RPWM2
  ledcAttachPin(LPWM2, 3);  // canal 3 -> pino LPWM2
}

// Converte a velocidade limite de % (0 a 100) para PWM (0 a 255)
int vel() {
  return velPct * 255 / 100;
}

// Recalcula o PWM máximo permitido de acordo com a direção e a velocidade da página Web
void atualizarAlvos() {
  int v = vel();
  switch (sentidoAtual) {
    case 'F': targetA = v; targetB = 0; targetC = v; targetD = 0; break; // Frente
    case 'T': targetA = 0; targetB = v; targetC = 0; targetD = v; break; // Trás
    case 'E': targetA = v; targetB = 0; targetC = 0; targetD = v; break; // Esquerda
    case 'D': targetA = 0; targetB = v; targetC = v; targetD = 0; break; // Direita
    case 'P': targetA = 0; targetB = 0; targetC = 0; targetD = 0; break; // Parado
  }
}

// Funções chamadas pelos comandos Web apenas definem a intenção de movimento
void frente()   { sentidoAtual = 'F'; atualizarAlvos(); }
void tras()     { sentidoAtual = 'T'; atualizarAlvos(); }
void esquerda() { sentidoAtual = 'E'; atualizarAlvos(); }
void direita()  { sentidoAtual = 'D'; atualizarAlvos(); }
void parado()   { sentidoAtual = 'P'; atualizarAlvos(); }

// Calcula a transição suave de um valor PWM atual até o valor alvo
float aproximar(float atual, int alvo, float passo) {
  if (atual < alvo) {
    atual += passo;
    if (atual > alvo) atual = alvo;
  } else if (atual > alvo) {
    atual -= passo;
    if (atual < alvo) atual = alvo;
  }
  return atual;
}

// Função contínua que aplica o PWM gradualmente sem travar o processador
void atualizarAceleracao() {
  unsigned long agora = millis();
  if (agora - tempoAnteriorAcel >= INTERVALO_ACEL) {
    tempoAnteriorAcel = agora;

    currentA = aproximar(currentA, targetA, PASSO_ACEL);
    currentB = aproximar(currentB, targetB, PASSO_ACEL);
    currentC = aproximar(currentC, targetC, PASSO_ACEL);
    currentD = aproximar(currentD, targetD, PASSO_ACEL);

    ledcWrite(0, (int)currentA);
    ledcWrite(1, (int)currentB);
    ledcWrite(2, (int)currentC);
    ledcWrite(3, (int)currentD);
  }
}