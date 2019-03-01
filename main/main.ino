#include <osapi.h>
#include <user_interface.h>
#include <WebSocketsClient.h>

#define USE_SERIAL Serial

const uint8_t LED_PIN = 0;
const uint8_t SENSOR_D_PIN = 5;
const uint8_t PUMP_PIN = 4;

String appName = "SmartCirculationPump";
String apInitPass = "pametnapumpa82";
String revision = getRevision();

const uint32_t MODULE_UID = spi_flash_get_id();

boolean isConnected = false;  

void setup()
{
  randomSeed(analogRead(0));
  Serial.begin(115200);
  printAppSignature(appName);

  isConnected = setupWifi(appName);
  
  if (isConnected) {
    setupOTA(appName);
  }
  setupRegulation();
  setupWebSocket();
  // Indicate program started
  flashLed(3);

  digitalWrite(LED_PIN, HIGH);
  delay(200);
}

void loop()
{
  printAppSignature(appName);
  if (isConnected) {
    otaIteration();
  }
  regulationIteration();
  webSocketIteration();
}
