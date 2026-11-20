
#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include "board_config.h"

// =====================================================
// ROBOLIGHT - ESP32-CAM AI THINKER / OV2640
// Base: CameraWebServer oficial da Espressif
// Executa SOMENTE na ESP32-CAM, nao na ESP32 principal.
// =====================================================

const char* SSID  = "RoboLight";
const char* SENHA = "robolight123";

IPAddress ipCamera(192, 168, 4, 100);
IPAddress gateway(192, 168, 4, 1);
IPAddress mascara(255, 255, 255, 0);

// Ajustes iniciais (podem ser alterados pela interface oficial)
const framesize_t RESOLUCAO_INICIAL = FRAMESIZE_QVGA;
const int QUALIDADE_INICIAL = 20;

bool cameraPronta = false;
bool servidorIniciado = false;
uint32_t ultimaTentativaWiFi = 0;

// Implementadas no app_httpd.cpp original do CameraWebServer.
void startCameraServer();
void setupLedFlash();

bool iniciarCamera() {
  camera_config_t config = {};

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.jpeg_quality = QUALIDADE_INICIAL;

  if (psramFound()) {
    // A alocacao para UXGA permite escolher resolucoes maiores
    // posteriormente na interface web, como no exemplo oficial.
    config.frame_size = FRAMESIZE_UXGA;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Fallback para placas sem PSRAM.
    config.frame_size = FRAMESIZE_QQVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.fb_count = 1;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    Serial.println("Aviso: PSRAM nao detectada; resolucao limitada.");
  }

  esp_err_t erro = esp_camera_init(&config);
  if (erro != ESP_OK) {
    Serial.printf("Erro ao inicializar camera: 0x%x\n", erro);
    return false;
  }

  sensor_t* sensor = esp_camera_sensor_get();
  if (sensor == nullptr) {
    Serial.println("Erro: sensor nao detectado.");
    esp_camera_deinit();
    return false;
  }

  framesize_t resolucao = psramFound()
    ? RESOLUCAO_INICIAL
    : FRAMESIZE_QQVGA;

  if (sensor->set_framesize(sensor, resolucao) != 0) {
    Serial.println("Erro ao configurar resolucao inicial.");
    esp_camera_deinit();
    return false;
  }

  sensor->set_quality(sensor, QUALIDADE_INICIAL);

#if defined(LED_GPIO_NUM)
  setupLedFlash();
#endif

  return true;
}

void iniciarConexaoWiFi() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);

  if (!WiFi.config(ipCamera, gateway, mascara)) {
    Serial.println("Erro ao configurar IP fixo.");
  }

  WiFi.begin(SSID, SENHA);
}

void iniciarServidorSeConectado() {
  if (!cameraPronta || servidorIniciado || WiFi.status() != WL_CONNECTED) {
    return;
  }

  // Interface oficial na porta 80 e MJPEG na porta 81.
  startCameraServer();
  servidorIniciado = true;

  Serial.println("Camera pronta.");
  Serial.println("Painel: http://192.168.4.100/");
  Serial.println("Stream: http://192.168.4.100:81/stream");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  cameraPronta = iniciarCamera();
  if (!cameraPronta) {
    return;
  }

  iniciarConexaoWiFi();

  // Tempo limite apenas para a conexao inicial.
  uint32_t inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 15000) {
    delay(250);
  }

  iniciarServidorSeConectado();
}

void loop() {
  if (!cameraPronta) {
    delay(2000);
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    iniciarServidorSeConectado();
  } else if (millis() - ultimaTentativaWiFi >= 5000) {
    ultimaTentativaWiFi = millis();
    WiFi.reconnect();
  }

  delay(250);
}
