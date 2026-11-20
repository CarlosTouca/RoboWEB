// ===== Controle dos motores com duas pontes H (BTS7960) e aceleração suave =====
// Cada motor tem UMA velocidade com sinal: positivo = frente, negativo = trás.
// Só um dos dois canais da ponte recebe PWM por vez (o outro fica em 0).
const int FREQ = 5000;   // frequência do PWM em Hz
const int RES  = 8;      // resolução de 8 bits (valores de 0 a 255)
int velPct = 100;        // velocidade em % (0 a 100), muda pela barra da página

// Pinos da ponte 1 (motor esquerdo) e da ponte 2 (motor direito)
const int RPWM1 = 26;  // ponte 1, frente
const int LPWM1 = 27;  // ponte 1, trás
const int RPWM2 = 5;   // ponte 2, frente
const int LPWM2 = 17;  // ponte 2, trás

// Nomes por motor e sentido
const int ESQ_FRENTE = RPWM1, ESQ_TRAS = LPWM1;
const int DIR_FRENTE = RPWM2, DIR_TRAS = LPWM2;

// PWM mínimo para o motor sair do lugar. Se o robô não anda em velocidades baixas, aumente (ex.: 60)
const int PWM_MIN = 0;

// --- Variáveis de aceleração (valores de -255 a 255) ---
int alvoEsq = 0, alvoDir = 0;           // velocidade que queremos
int atualEsq = 0, atualDir = 0;         // velocidade que o motor tem agora
char sentidoAtual = 'P';                // direção atual ('F', 'T', 'E', 'D', 'P')

unsigned long tempoAnteriorAcel = 0;
const int INTERVALO_ACEL = 10;   // Intervalo de atualização da rampa (ms)
const int PASSO_ACEL     = 4;    // Incremento de PWM por passo (aumente para acelerar mais rápido)

// Prepara o PWM dos 4 pinos (core 3.x: ledcAttach faz o papel de ledcSetup + ledcAttachPin)
void Init_PonteH() {
  ledcAttach(ESQ_FRENTE, FREQ, RES);
  ledcAttach(ESQ_TRAS,   FREQ, RES);
  ledcAttach(DIR_FRENTE, FREQ, RES);
  ledcAttach(DIR_TRAS,   FREQ, RES);
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
    case 'F': alvoEsq = -v; alvoDir = -v; break;  // Frente
    case 'T': alvoEsq =  v; alvoDir =  v; break;  // Trás
    case 'E': alvoEsq =  v; alvoDir = -v; break;  // Gira para a esquerda (se girar ao contrário, troque os sinais destas duas linhas)
    case 'D': alvoEsq = -v; alvoDir =  v; break;  // Gira para a direita
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

// Aplica um valor com sinal em um motor: zera o sentido oposto ANTES de ligar o outro
void escreveMotor(int pinoFrente, int pinoTras, int valor) {
  if (valor > 0) {
    ledcWrite(pinoTras, 0);
    ledcWrite(pinoFrente, valor);
  } else if (valor < 0) {
    ledcWrite(pinoFrente, 0);
    ledcWrite(pinoTras, -valor);
  } else {
    ledcWrite(pinoFrente, 0);
    ledcWrite(pinoTras, 0);
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
