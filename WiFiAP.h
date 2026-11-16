// ===== A ESP32 cria a própria rede Wi-Fi (modo Access Point) =====
const char* AP_SSID  = "RoboLight";     // nome da rede que vai aparecer no celular/PC
const char* AP_SENHA = "robolight123";  // senha da rede (mínimo 8 caracteres)
const int AP_CANAL        = 1;          // canal Wi-Fi (1, 6 ou 11 costumam ter menos interferência)
const int AP_MAX_CLIENTES = 4;          // limite de aparelhos conectados ao mesmo tempo

void Init_wifi() {
  WiFi.mode(WIFI_AP);                // a ESP vira o roteador (não procura outra rede)
  WiFi.softAP(AP_SSID, AP_SENHA, AP_CANAL, 0, AP_MAX_CLIENTES);  // cria a rede com o nome, a senha, o canal e o limite acima
  Serial.print("Rede criada. IP: ");
  Serial.println(WiFi.softAPIP());   // IP da ESP, normalmente 192.168.4.1
}
