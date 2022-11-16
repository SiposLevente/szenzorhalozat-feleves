#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "headers/sensor_device.h"
#include <Arduino.h>
#define TEMPERATURE_SERVICE_UUID "e414d508-cb57-40d5-bac5-0d237078f34f"
#define GATEWAY_COMMUNICATAION_MEASURMENTS_PER_MINUTE_SERVICE "ee459751-dc64-474a-8a5f-ada5cdf9c5ed"
#define GATEWAY_COMMUNICATAION_CALCULATION_MODE_SERVICE "509093fd-5d42-4631-ba05-69206758a883"
#define CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593d"

SensorDevice device = SensorDevice();
void SetCalculationMode(const char *new_calc_mode);
void SetDeviceMeasurementPerMinute(int new_measurement_per_minute);


class BLEMeasurementsPerMinuteCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic)
  {
    const char *value = characteristic->getValue().c_str();
    Serial.printf("MPM: %d\n", value);
    SetDeviceMeasurementPerMinute(atoi(value));
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
    Serial.printf("CALC MODE: %d\n", value);
    SetCalculationMode(value);
  }
  void onRead(BLECharacteristic *characteristic)
  {
    int measaurements_per_minute = device.GetMeasurementsPerMinute();
    characteristic->setValue(measaurements_per_minute);
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
  BLEDevice::init(device.GetID());
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pTemperatureService = pServer->createService(TEMPERATURE_SERVICE_UUID);
  BLEService *pCalculationModeService = pServer->createService(GATEWAY_COMMUNICATAION_CALCULATION_MODE_SERVICE);
  BLEService *pMeasurementsPerMinuteService = pServer->createService(GATEWAY_COMMUNICATAION_MEASURMENTS_PER_MINUTE_SERVICE);

  BLECharacteristic *pCalculationModeCharacteristic = pCalculationModeService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pMeasurementPerMinuteCharacteristic = pMeasurementsPerMinuteService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pTempCharacteristic = pTemperatureService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ);

  pCalculationModeCharacteristic->setCallbacks(new BLECalculationModeCallback());
  pCalculationModeCharacteristic->setValue(CALCULATION_MODE_AVERAGE_TYPE_TEXT);
  pMeasurementPerMinuteCharacteristic->setCallbacks(new BLEMeasurementsPerMinuteCallback());
  //=============================
  char *value;
  itoa(DEFAULT_MEASUREMENT_PER_MINUTE_VALUE, value, 10);
  //=============================
  pMeasurementPerMinuteCharacteristic->setValue(value);
  pTempCharacteristic->setCallbacks(new BLETemperatureCallback());
  pTempCharacteristic->setValue("-1");

  pTemperatureService->start();
  pCalculationModeService->start();
  pMeasurementsPerMinuteService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(TEMPERATURE_SERVICE_UUID);
  pAdvertising->addServiceUUID(GATEWAY_COMMUNICATAION_CALCULATION_MODE_SERVICE);
  pAdvertising->addServiceUUID(GATEWAY_COMMUNICATAION_MEASURMENTS_PER_MINUTE_SERVICE);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void loop()
{
  if (device.canCollectData())
  {
    device.CollecData();
  }
  device.Sleep();
}

void SetDeviceMeasurementPerMinute(int new_measurement_per_minute)
{
  device.SetMeasurementPerMinute(new_measurement_per_minute);
}

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
