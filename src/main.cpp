#include <Arduino.h>

#include "ConfigManager.h"


ConfigManager configManager;

void setup() {
  Serial.begin(115200);

  configManager.load_config();
}

void loop() {
}
