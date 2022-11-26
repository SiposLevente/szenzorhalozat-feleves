#include <Arduino.h>
#include <BLEDevice.h>
#include "headers/sensor_device.h"

// --------------------- Remote Service UUID ---------------------
#define REMOTE_SERVICE_UUID "e414d508-cb57-40d5-bac5-0d237078f34f"

// --------------------- Characteristics' UUID ---------------------
#define TEMPERATURE_REMOTE_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593b"
#define CALCULATION_MODE_REMOTE_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593c"
#define MEASUREMENTS_PER_MINUTE_REMOTE_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593d"

// States
bool doConnect = false;
bool isConnected = false;
bool doScan = false;
bool isNotifySet = false;

// Notification constant
const uint8_t notificationOn[] = {0x1, 0x0};

// Function declarations
void CalculationModeNotifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
void MeasurementPerMinuteNotifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
void SetCalculationMode(const char *new_calc_mode);
void SetDeviceMeasurementPerMinute(int new_measurement_per_minute);

// Characteristics' declaration
BLERemoteCharacteristic *pTemperatureRemoteCharacteristic;
BLERemoteCharacteristic *pMeasurmentsPerMinuteRemoteCharacteristic;
BLERemoteCharacteristic *pCalculationModeRemoteCharacteristic;

BLEAdvertisedDevice *myDevice;

SensorDevice device = SensorDevice();

// Connection callback
class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pclient)
  {
  }
  void onDisconnect(BLEClient *pclient)
  {
    isConnected = false;
    Serial.println("Disconnected from the server!");
  }
};

bool connectToServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println("Created client!");
  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remote BLE Server.
  pClient->connect(myDevice);
  Serial.println("Connected to server!");

  // Remote service
  BLERemoteService *RemoteService = pClient->getService(REMOTE_SERVICE_UUID);

  if (RemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(REMOTE_SERVICE_UUID);
    pClient->disconnect();
    return false;
  }
  Serial.println("Found remote service!");

  // ----------------------- Temperature characteristic -----------------------
  pTemperatureRemoteCharacteristic = RemoteService->getCharacteristic(TEMPERATURE_REMOTE_CHARACTERISTIC_UUID);
  if (pTemperatureRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(TEMPERATURE_REMOTE_CHARACTERISTIC_UUID);
    pClient->disconnect();
    return false;
  }
  Serial.println("Found temperature characteristic!");

  // ----------------------- Measurement per minute characteristic -----------------------

  pMeasurmentsPerMinuteRemoteCharacteristic = RemoteService->getCharacteristic(MEASUREMENTS_PER_MINUTE_REMOTE_CHARACTERISTIC_UUID);
  if (pMeasurmentsPerMinuteRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(MEASUREMENTS_PER_MINUTE_REMOTE_CHARACTERISTIC_UUID);
    pClient->disconnect();
    return false;
  }
  Serial.println("Found measurement per minute characteristic!");

  // ----------------------- Calculation mode characteristic -----------------------
  pCalculationModeRemoteCharacteristic = RemoteService->getCharacteristic(CALCULATION_MODE_REMOTE_CHARACTERISTIC_UUID);
  if (pCalculationModeRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(CALCULATION_MODE_REMOTE_CHARACTERISTIC_UUID);
    pClient->disconnect();
    return false;
  }
  Serial.println("Found calculation mode characteristic!");
  // ----------------------- Reading and writing characteristics -----------------------

  if (pMeasurmentsPerMinuteRemoteCharacteristic->canRead())
  {
    SetDeviceMeasurementPerMinute(atoi(pMeasurmentsPerMinuteRemoteCharacteristic->readValue().c_str()));
  }

  if (pCalculationModeRemoteCharacteristic->canRead())
  {
    SetCalculationMode(pCalculationModeRemoteCharacteristic->readValue().c_str());
  }

  if (pTemperatureRemoteCharacteristic->canWrite())
  {
    pTemperatureRemoteCharacteristic->writeValue(device.GetData());
  }

  // Setting notify callbacks
  if (!isNotifySet)
  {
    pCalculationModeRemoteCharacteristic->registerForNotify(CalculationModeNotifyCallback);
    pMeasurmentsPerMinuteRemoteCharacteristic->registerForNotify(MeasurementPerMinuteNotifyCallback);
    isNotifySet = true;
  }
  isConnected = true;
  return true;
}

// Callback functions
void CalculationModeNotifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
  SetCalculationMode((char *)pData);
}

void MeasurementPerMinuteNotifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
  SetDeviceMeasurementPerMinute(atoi((char *)pData));
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {

    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.isAdvertisingService(BLEUUID(REMOTE_SERVICE_UUID)));
    // Found a device
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(REMOTE_SERVICE_UUID)))
    {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    } // Found our server

  } // onResult
};

void setup()
{
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

// Loop holds the main logic of the sensor management.
void loop()
{
  if (doConnect)
  {
    if (connectToServer())
    {
      Serial.println("Connected to the BLE Server.");

      // Setting remote notification
      pMeasurmentsPerMinuteRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)notificationOn, 2, true);
      pCalculationModeRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)notificationOn, 2, true);
      pTemperatureRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)notificationOn, 2, true);
      doConnect = false;
    }
    else
    {
      Serial.println("Failed to connect to the server! Retrying!");
      delay(1000);
    }
  }
  if (isConnected)
  {
    // Main logic of the sensor device
    if (device.canCollectData())
    {
      device.CollecData();
    }
    if (device.canSendData())
    {
      device.ProcessData();
      char tempDataBuffer[64];
      snprintf(tempDataBuffer, sizeof tempDataBuffer, "%0.2f", device.GetData());
      pTemperatureRemoteCharacteristic->writeValue(tempDataBuffer);
    }
    device.Sleep();
  }
  else if (doScan)
  {
    BLEDevice::getScan()->start(0);
  }
}

// Used by callbacks, sets the sensor device's measurement per minute value
void SetDeviceMeasurementPerMinute(int new_measurement_per_minute)
{
  if (new_measurement_per_minute > 0 && new_measurement_per_minute <= 120000)
  {
    device.SetMeasurementPerMinute(new_measurement_per_minute);
  }
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
