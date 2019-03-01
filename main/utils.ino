#include <WebSocketsClient.h>
#include <WebSockets.h>

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
  {
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

    // send message to server when Connected
    webSocket.sendTXT("Connected");
  }
  break;
  case WStype_TEXT:
    USE_SERIAL.printf("[WSc] get text: %s\n", payload);

    // send message to server
    webSocket.sendTXT("message here");
    break;
  case WStype_BIN:
    USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);

    // send data to server
    webSocket.sendBIN(payload, length);
    break;
  }
}

void setupWebSocket()
{
  // server address, port and URL
  webSocket.begin("192.168.1.45", 81, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  // webSocket.setAuthorization("admin", "admin");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  // webSocket.enableHeartbeat(15000, 3000, 2);
}

void webSocketIteration()
{
  webSocket.loop();
}



String getPrettyTimestamp(uint32_t ts)
{
  return String("[ " + String((float)(ts0 / 1000) + (0.001 * (ts0 % 1000)), 3) + "s ] ");
}

void printTempLog(
    uint32_t timestamp,
    float tempSensorsDiff,
    float tempDiffVelocity,
    float tempSensorDiffVelocityNorm,
    boolean punpState)
{
  Serial.print(getPrettyTimestamp(timestamp));
  Serial.print(" Temp. sensors diff.: ");
  Serial.println(tempSensorsDiff);
  Serial.print("Temp. sensor diff. / read: ");
  Serial.println(tempDiffVelocity);
  Serial.print("Temp. velocity: ");
  Serial.println(tempSensorDiffVelocityNorm);
  Serial.println(pumpState ? "Pump ON" : "Pump OFF");
  Serial.println();
}

void printAppSignature(String name)
{
  Serial.printf(
      "%s v0.0.1-%s%s (chipid: %08X, flashid: %08X)",
      name,
      __DATE__,
      __TIME__,
      ESP.getChipId(),
      MODULE_UID);
}

void flashLed(int timesRepeat)
{
  for (uint8_t i = 0; i < timesRepeat; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

uint8_t findDevices(int pin)
{
  OneWire ow(pin);

  uint8_t address[8];
  uint8_t count = 0;

  if (ow.search(address))
  {
    Serial.print("\nuint8_t pin");
    Serial.print(pin, DEC);
    Serial.println("[][8] = {");
    do
    {
      count++;
      Serial.println("  {");
      for (uint8_t i = 0; i < 8; i++)
      {
        Serial.print("0x");
        if (address[i] < 0x10)
          Serial.print("0");
        Serial.print(address[i], HEX);
        if (i < 7)
          Serial.print(", ");
      }
      Serial.println("  },");
    } while (ow.search(address));

    Serial.println("};");
    Serial.print("// nr devices found: ");
    Serial.println(count);
  }

  return count;
}