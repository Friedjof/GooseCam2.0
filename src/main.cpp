#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <I2S.h>

#include "esp_camera.h"
#include "camera_pins.h"

#include "ConfigManager.h"

// Audio record time setting (in seconds, max value 240)
#define RECORD_TIME 10
 
// Audio settings
#define SAMPLE_RATE 16000U
#define SAMPLE_BITS 16
#define WAV_HEADER_SIZE 44
#define VOLUME_GAIN 3

void setup_camera();
void take_picture();


ConfigManager configManager;
AsyncWebServer server(80);

camera_config_t config;


void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(10);
  }

  // Initialize I2S
  I2S.setAllPins(-1, 42, 41, -1, -1);
  if (!I2S.begin(PDM_MONO_MODE, SAMPLE_RATE, SAMPLE_BITS)) {
    Serial.println("Failed to initialize I2S!");
    while (1) {
      delay(10);
      Serial.println("Failed to initialize I2S!");
    }
  }
  Serial.println("I2S initialized");

  // Initialize the MicroSD card
  if (!SD.begin(21)) {
    Serial.println("Failed to mount MicroSD Card!");
    while (1) {
      delay(10);
      Serial.println("Failed to mount MicroSD Card!");
    }
  }
  Serial.println("MicroSD Card mounted");

    // Determine what type of MicroSD card is mounted
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No MicroSD card inserted!");
    return;
  }

  // Print card type
  Serial.print("MicroSD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  configManager.load_config();

  Serial.println("Connecting to WiFi: " + String(configManager.get_wifi_ssid()));

  // connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(configManager.get_wifi_ssid(), configManager.get_wifi_password());

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  setup_camera();

  // routes
  server.on("/picture", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Taking picture...");
    take_picture();

    request->send(SD, "/picture.jpg", "image/jpeg");
  });

  server.begin();
}

void loop() { }

void setup_camera() {
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 2;
  config.grab_mode = CAMERA_GRAB_LATEST;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err) {
    Serial.println("Camera init failed with error: " + String(err));
  }
}

void take_picture() {
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  File file = SD.open("/picture.jpg", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
    return;
  }

  file.write(fb->buf, fb->len);
  file.close();
  esp_camera_fb_return(fb);
  Serial.println("Picture saved");
}