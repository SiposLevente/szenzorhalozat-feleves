#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "headers/sensor_device.h"
#include <Arduino.h>

// --------------------- Services' UUID ---------------------
#define TEMPERATURE_SERVICE_UUID "e414d508-cb57-40d5-bac5-0d237078f34f"
#define GATEWAY_COMMUNICATAION_MEASURMENTS_PER_MINUTE_SERVICE "ee459751-dc64-474a-8a5f-ada5cdf9c5ed"
#define GATEWAY_COMMUNICATAION_CALCULATION_MODE_SERVICE "509093fd-5d42-4631-ba05-69206758a883"

// --------------------- Characteristics' UUID ---------------------
#define TEMP_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593b"
#define CALC_MODE_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593c"
#define MEASUREMENTS_PER_MINUTE_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593d"

// --------------------- SensorDevice creation ---------------------
SensorDevice device = SensorDevice();

// --------------------- Function declaration ---------------------
void SetCalculationMode(const char *new_calc_mode);
void SetDeviceMeasurementPerMinute(int new_measurement_per_minute);

// --------------------- BLE characteristics' callback declaration ---------------------
class BLEMeasurementsPerMinuteCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic)
  {
    int value = atoi(characteristic->getValue().c_str());
    SetDeviceMeasurementPerMinute(value);
  }
  void onRead(BLECharacteristic *characteristic)
  {
    int measaurements_per_minute = device.GetMeasurementsPerMinute();
    characteristic->setValue(measaurements_per_minute);
  }
};

class BLECalculationModeCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic)
  {
    const char *value = characteristic->getValue().c_str();
    SetCalculationMode(value);
  }
  void onRead(BLECharacteristic *characteristic)
  {
    char *calculation_mode = device.GetCalculationMode();
    characteristic->setValue(calculation_mode);
  }
};

class BLETemperatureCallback : public BLECharacteristicCallbacks
{

  void onRead(BLECharacteristic *characteristic)
  {
    char buffer[10];
    if (device.canSendData())
    {
      device.ProcessData();
      dtostrf(device.GetData(), 4, 2, buffer);
      characteristic->setValue(buffer);
    }
  }
};

void setup()
{
  // Ble initialization.
  BLEDevice::init(device.GetID());
  BLEServer *pServer = BLEDevice::createServer();

  // Ble services' creation.
  BLEService *pTemperatureService = pServer->createService(TEMPERATURE_SERVICE_UUID);
  BLEService *pCalculationModeService = pServer->createService(GATEWAY_COMMUNICATAION_CALCULATION_MODE_SERVICE);
  BLEService *pMeasurementsPerMinuteService = pServer->createService(GATEWAY_COMMUNICATAION_MEASURMENTS_PER_MINUTE_SERVICE);

  // Ble characteristics' creation.
  BLECharacteristic *pCalculationModeCharacteristic = pCalculationModeService->createCharacteristic(
      CALC_MODE_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pMeasurementPerMinuteCharacteristic = pMeasurementsPerMinuteService->createCharacteristic(
      MEASUREMENTS_PER_MINUTE_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pTempCharacteristic = pTemperatureService->createCharacteristic(
      TEMP_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ);

  // Setting callbacks and initial values.
  pCalculationModeCharacteristic->setCallbacks(new BLECalculationModeCallback());
  pCalculationModeCharacteristic->setValue(CALCULATION_MODE_AVERAGE_TYPE_TEXT);

  pMeasurementPerMinuteCharacteristic->setCallbacks(new BLEMeasurementsPerMinuteCallback());
  char value[5];
  itoa(DEFAULT_MEASUREMENT_PER_MINUTE_VALUE, value, 10);
  pMeasurementPerMinuteCharacteristic->setValue(value);

  pTempCharacteristic->setCallbacks(new BLETemperatureCallback());
  pTempCharacteristic->setValue("-1");

  // Starting BLE services.
  pTemperatureService->start();
  pCalculationModeService->start();
  pMeasurementsPerMinuteService->start();

  // Creating, configuring, and starting BLE advertising.
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(TEMPERATURE_SERVICE_UUID);
  pAdvertising->addServiceUUID(GATEWAY_COMMUNICATAION_CALCULATION_MODE_SERVICE);
  pAdvertising->addServiceUUID(GATEWAY_COMMUNICATAION_MEASURMENTS_PER_MINUTE_SERVICE);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

// Loop holds the main logic of the sensor management.
void loop()
{
  if (device.canCollectData())
  {
    device.CollecData();
  }
  device.Sleep();
}

// Used by callbacks, sets the sensor device's measurement per minute value
void SetDeviceMeasurementPerMinute(int new_measurement_per_minute)
{
  device.SetMeasurementPerMinute(new_measurement_per_minute);
}

// Used by callbacks, sets the sensor device's callculation mode value
void SetCalculationMode(const char *new_calc_mode)
{
  CalculationMode calc_mode;
  if (new_calc_mode == CALCULATION_MODE_AVERAGE_TYPE_TEXT)
  {
    calc_mode = CalculationMode::Average;
  }
  else if (new_calc_mode == CALCULATION_MODE_MEDIAN_TYPE_TEXT)
  {
    calc_mode = CalculationMode::Median;
  }
  else
  {
    calc_mode = CalculationMode::Mode;
  }

  device.SetCalculationMode(calc_mode);
}
