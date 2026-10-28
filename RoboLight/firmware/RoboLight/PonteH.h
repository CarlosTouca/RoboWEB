// ===== Controle dos motores com duas pontes H (BTS7960) =====
const int FREQ = 5000;   // frequência do PWM em Hz
const int RES  = 8;      // resolução de 8 bits (valores de 0 a 255)
int velPct = 100;        // velocidade em % (0 a 100), muda pela barra da página

// Pinos da ponte 1 (motor esquerdo) e da ponte 2 (motor direito)
const int RPWM1 = 18;  // ponte 1, sentido 1
const int LPWM1 = 19;  // ponte 1, sentido 2
const int RPWM2 = 5;   // ponte 2, sentido 1
const int LPWM2 = 17;  // ponte 2, sentido 2

// Prepara os 4 canais de PWM e liga cada um a um pino
void Init_PonteH() {
  for (int c = 0; c < 4; c++) ledcSetup(c, FREQ, RES); // configura os canais 0, 1, 2 e 3
  ledcAttachPin(RPWM1, 0);  // canal 0 -> pino RPWM1
  ledcAttachPin(LPWM1, 1);  // canal 1 -> pino LPWM1
  ledcAttachPin(RPWM2, 2);  // canal 2 -> pino RPWM2
  ledcAttachPin(LPWM2, 3);  // canal 3 -> pino LPWM2
}

// Converte a velocidade de % (0 a 100) para PWM (0 a 255)
int vel() {
  return velPct * 255 / 100;
}

// Escreve a velocidade nos 4 canais de uma vez (a, b = ponte 1; c, d = ponte 2)
void mover(int a, int b, int c, int d) {
  ledcWrite(0, a);
  ledcWrite(1, b);
  ledcWrite(2, c);
  ledcWrite(3, d);
}

void frente()   { mover(vel(), 0, vel(), 0); }   // os dois motores para frente
void tras()     { mover(0, vel(), 0, vel()); }   // os dois motores para trás
void esquerda() { mover(vel(), 0, 0, vel()); }   // motores em sentidos opostos: gira para a esquerda
void direita()  { mover(0, vel(), vel(), 0); }   // motores em sentidos opostos: gira para a direita
void parado()   { mover(0, 0, 0, 0); }           // tudo em zero: robô para
