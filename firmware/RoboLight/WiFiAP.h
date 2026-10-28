// ===== A ESP32 cria a própria rede Wi-Fi (modo Access Point) =====
const char* AP_SSID  = "RoboLight";     // nome da rede que vai aparecer no celular/PC
const char* AP_SENHA = "robolight123";  // senha da rede (mínimo 8 caracteres)

void Init_wifi() {
  WiFi.mode(WIFI_AP);                // a ESP vira o roteador (não procura outra rede)
  WiFi.softAP(AP_SSID, AP_SENHA);    // cria a rede com o nome e a senha acima
  Serial.print("Rede criada. IP: ");
  Serial.println(WiFi.softAPIP());   // IP da ESP, normalmente 192.168.4.1
}
