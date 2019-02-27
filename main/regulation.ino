#include <OneWire.h>
#include <DallasTemperature.h>

/* DS18S20 Temperature chip i/o */
OneWire ds(SENSOR_D_PIN);
DallasTemperature sensors(&ds);

// Threshold constants in Celsius
float Hysteresis = 0.5;
float OnTempDiff = 0.3;
float InitOnTempDiff = 2;

uint32_t MAX_PUMP_ON_MS = 60000;
uint32_t ts0 = 0;
uint32_t pumpStartTs = 0;
float lastTd = 2.34E5;

bool pumpState = false;
bool firstIteration = true;

/*
  TODO: handle initial state when time diff at the beginning eg.:

  Requesting temperatures...DONE
  Temperature for the device 1 (index 0) is: 20.87
  Temperature for the device 2 (index 1) is: 23.44
  Temp. sensors diff.: 2.56
  Temp. sensor diff. / read: 0.00
  Temp. velocity: 0.00

  TODO: handle outputs OFF while upgrading !!
  TODO: evaluate in real environment
*/

void setupRegulation()
{
  sensors.begin();
  findDevices(SENSOR_D_PIN);
  pinMode(PUMP_PIN, OUTPUT);
  toggleRelay(false);

  Serial.print("OneWire sensors on PIN: ");
  Serial.println(SENSOR_D_PIN);
}

void regulationIteration()
{
  ts0 = millis();
  delay(1000);

  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures... ");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC1 = sensors.getTempCByIndex(0);
  float tempC2 = sensors.getTempCByIndex(1);

  // Check if reading was successful
  if (tempC1 != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC1);
  }
  else
  {
    Serial.println("Error: Could not read sensor 1 temperature data");
  }
  if (tempC2 != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature for the device 2 (index 1) is: ");
    Serial.println(tempC2);
  }
  else
  {
    Serial.println("Error: Could not read sensor 2 temperature data");
  }

  if (tempC1 != DEVICE_DISCONNECTED_C && tempC2 != DEVICE_DISCONNECTED_C)
  {
    float Td = tempC2 - tempC1;
    float Tdt = lastTd == 2.34E5
                    ? 0
                    : Td - lastTd;
    uint32_t Tsd = abs(millis() - ts0);
    float Tvd = 1000 * Tdt / (float)Tsd;
    lastTd = Td;

    printTempLog(ts0, Td, Tdt, Tvd, pumpState);

    if (!pumpState && (Tvd > OnTempDiff || (firstIteration && Td > InitOnTempDiff)))
    {
      Serial.print(getPrettyTimestamp(ts0));
      toggleRelay(true);
      pumpState = true;
      pumpStartTs = ts0;
    }

    if (pumpState && (Tvd < (-1 * OnTempDiff) || (ts0 - pumpStartTs) > MAX_PUMP_ON_MS))
    {
      pumpState = false;
      toggleRelay(false);
      Serial.print(getPrettyTimestamp(ts0));
      Serial.print(ts0 - pumpStartTs);
      Serial.print("s) =============\n");
    }
  }

  firstIteration = false;
}

void toggleRelay(boolean on)
{
  if (on)
  {
    Serial.print(" ============= PUMP STARTED =============\n");
  }
  else
  {
    Serial.print(" ============= PUMP STOPPED (");
  }
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  digitalWrite(PUMP_PIN, on ? LOW : HIGH);
}