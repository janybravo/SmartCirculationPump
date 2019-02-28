
String getPrettyTimestamp(uint32_t ts)
{
  return String("[ " + String((float)(ts0 / 1000) + (0.001 * (ts0 % 1000)), 3) + "s ] ");
}

void printTempLog(
  uint32_t timestamp, 
  float tempSensorsDiff, 
  float tempDiffVelocity,
  float tempSensorDiffVelocityNorm,
  boolean punpState
  )
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

void printAppSignature(String name) {
  Serial.print("\nSetup ");
  Serial.print(name);
  Serial.printf(" v0.0.1- (chipid: %08X, flashid: %08X)", ESP.getChipId(), spi_flash_get_id());
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