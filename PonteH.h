// ===== Controle dos motores com duas pontes H (BTS7960) e aceleração suave =====
// Cada motor tem UMA velocidade com sinal: positivo = frente, negativo = trás.
// Só um dos dois canais da ponte recebe PWM por vez (o outro fica em 0).
//const int FREQ = 5000;   // frequência do PWM em Hz
const int RES  = 8;      // resolução de 8 bits (valores de 0 a 255)
int velPct = 100;        // velocidade em % (0 a 100), muda pela barra da página

// Pinos da ponte 1 (motor esquerdo) e da ponte 2 (motor direito)
const int RPWM1 = 18;  // ponte 1, frente
const int LPWM1 = 19;  // ponte 1, trás
const int RPWM2 = 5;   // ponte 2, frente
const int LPWM2 = 17;  // ponte 2, trás

// Canal PWM de cada sentido
const int ESQ_FRENTE = 0, ESQ_TRAS = 1;
const int DIR_FRENTE = 2, DIR_TRAS = 3;

// PWM mínimo para o motor sair do lugar. Se o robô não anda em velocidades baixas, aumente (ex.: 60)
const int PWM_MIN = 0;

// --- Variáveis de aceleração (valores de -255 a 255) ---
int alvoEsq = 0, alvoDir = 0;           // velocidade que queremos
int atualEsq = 0, atualDir = 0;         // velocidade que o motor tem agora
char sentidoAtual = 'P';                // direção atual ('F', 'T', 'E', 'D', 'P')

unsigned long tempoAnteriorAcel = 0;
const int INTERVALO_ACEL = 10;   // Intervalo de atualização da rampa (ms)
const int PASSO_ACEL     = 4;    // Incremento de PWM por passo (aumente para acelerar mais rápido)

// Prepara os 4 canais de PWM e liga cada um a um pino
void Init_PonteH() {
  for (int c = 0; c < 4; c++) ledcSetup(c, FREQ, RES);
  ledcAttachPin(RPWM1, ESQ_FRENTE);
  ledcAttachPin(LPWM1, ESQ_TRAS);
  ledcAttachPin(RPWM2, DIR_FRENTE);
  ledcAttachPin(LPWM2, DIR_TRAS);
}

// Converte a velocidade limite de % (0 a 100) para PWM (PWM_MIN a 255)
int vel() {
  if (velPct == 0) return 0;
  return PWM_MIN + (255 - PWM_MIN) * velPct / 100;
}

// Recalcula as velocidades alvo de acordo com a direção e a velocidade da página
void atualizarAlvos() {
  int v = vel();
  switch (sentidoAtual) {
    case 'F': alvoEsq =  v; alvoDir =  v; break;  // Frente
    case 'T': alvoEsq = -v; alvoDir = -v; break;  // Trás
    case 'E': alvoEsq = -v; alvoDir =  v; break;  // Gira para a esquerda (se girar ao contrário, troque os sinais destas duas linhas)
    case 'D': alvoEsq =  v; alvoDir = -v; break;  // Gira para a direita
    default:  alvoEsq = 0;  alvoDir = 0;  break;  // Parado
  }
}

// Funções chamadas pelos comandos Web apenas definem a intenção de movimento
void frente()   { sentidoAtual = 'F'; atualizarAlvos(); }
void tras()     { sentidoAtual = 'T'; atualizarAlvos(); }
void esquerda() { sentidoAtual = 'E'; atualizarAlvos(); }
void direita()  { sentidoAtual = 'D'; atualizarAlvos(); }
void parado()   { sentidoAtual = 'P'; atualizarAlvos(); }   // para com rampa

// Calcula a transição suave de um valor PWM atual até o valor alvo
int aproximar(int atual, int alvo, int passo) {
  if (atual < alvo) {
    atual += passo;
    if (atual > alvo) atual = alvo;
  } else if (atual > alvo) {
    atual -= passo;
    if (atual < alvo) atual = alvo;
  }
  return atual;
}

// Aplica um valor com sinal em um motor: zera o canal oposto ANTES de ligar o outro
void escreveMotor(int canalFrente, int canalTras, int valor) {
  if (valor > 0) {
    ledcWrite(canalTras, 0);
    ledcWrite(canalFrente, valor);
  } else if (valor < 0) {
    ledcWrite(canalFrente, 0);
    ledcWrite(canalTras, -valor);
  } else {
    ledcWrite(canalFrente, 0);
    ledcWrite(canalTras, 0);
  }
}

// Parada imediata, sem rampa (emergência e perda de conexão)
void paradaImediata() {
  sentidoAtual = 'P';
  alvoEsq = 0;  alvoDir = 0;
  atualEsq = 0; atualDir = 0;
  escreveMotor(ESQ_FRENTE, ESQ_TRAS, 0);
  escreveMotor(DIR_FRENTE, DIR_TRAS, 0);
}

// Função contínua que aplica o PWM gradualmente sem travar o processador
void atualizarAceleracao() {
  unsigned long agora = millis();
  if (agora - tempoAnteriorAcel >= INTERVALO_ACEL) {
    tempoAnteriorAcel = agora;

    // Só mexe no PWM quando o valor mudou (parado ou já na velocidade alvo, não escreve nada)
    int novoEsq = aproximar(atualEsq, alvoEsq, PASSO_ACEL);
    if (novoEsq != atualEsq) {
      atualEsq = novoEsq;
      escreveMotor(ESQ_FRENTE, ESQ_TRAS, atualEsq);
    }

    int novoDir = aproximar(atualDir, alvoDir, PASSO_ACEL);
    if (novoDir != atualDir) {
      atualDir = novoDir;
      escreveMotor(DIR_FRENTE, DIR_TRAS, atualDir);
    }
  }
}

#ifdef TESTE_CANAIS
// Liga um canal por vez (rodas no ar!) e mostra no serial qual deveria estar girando
void testeCanais() {
  const char* nomes[4] = {"Esquerdo FRENTE (RPWM1)", "Esquerdo TRAS (LPWM1)",
                          "Direito FRENTE (RPWM2)",  "Direito TRAS (LPWM2)"};
  delay(3000);
  for (int c = 0; c < 4; c++) {
    Serial.print("Canal "); Serial.print(c); Serial.print(": "); Serial.println(nomes[c]);
    ledcWrite(c, 100);
    delay(2000);
    ledcWrite(c, 0);
    delay(500);
  }
  Serial.println("Teste terminado.");
}
#endif
