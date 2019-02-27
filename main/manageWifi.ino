#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

boolean setupWifi(String appName) {
  boolean isConnected = false;
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  // exit after config instead of connecting
  // wifiManager.setBreakAfterConfig(true);
  // reset settings - for testing
  // wifiManager.resetSettings();

  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //here appName with password "pametnapumpa82"
  //and goes into a blocking loop awaiting configuration
  const char *appName1 = appName.c_str();
  const char *apInitPass1 = apInitPass.c_str();
  if (!wifiManager.autoConnect(appName1, apInitPass1)) {
    Serial.println("failed to connect, we should reset as see if it connects");
    flashLed(1);
    delay(3000);
    ESP.reset();
    delay(5000);
  } else {
    flashLed(5);
    isConnected = true;
  }

  // TODO: safely persist local wifi password

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.print("Local ip: ");
  Serial.println(WiFi.localIP());

  return isConnected;
}