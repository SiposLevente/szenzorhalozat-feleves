#include <Arduino.h>
#include <BLEDevice.h>
#include "headers/sensor_device.h"

// --------------------- Remote Service UUID ---------------------
#define REMOTE_SERVICE_UUID "e414d508-cb57-40d5-bac5-0d237078f34f"

// --------------------- Characteristics' UUID ---------------------
#define TEMPERATURE_REMOTE_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593b"
#define CALCULATION_MODE_REMOTE_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593c"
#define MEASUREMENTS_PER_MINUTE_REMOTE_CHARACTERISTIC_UUID "b17516f7-0b89-4ade-9a84-0b849b3b593d"
#define ID_LENGTH 8
#define FLOAT_CONVERSION_ARRAY_LENGTH 8
#define SEPARATOR_CHAR_LENGTH 1

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
void SetDeviceMeasurementPerMinute(const char *new_measurement_per_minute);

bool ContainsDeviceId(const char *data);
char *RemoveDeviceIdFromData(const char *data);
char *AddDeviceId(float value);

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
    SetDeviceMeasurementPerMinute(pMeasurmentsPerMinuteRemoteCharacteristic->readValue().c_str());
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
  SetDeviceMeasurementPerMinute((char *)pData);
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
      char *data = AddDeviceId(device.GetData());
      Serial.printf("\nData sent: %s\n\n", data);
      pTemperatureRemoteCharacteristic->writeValue(data);
    }
    device.Sleep();
  }
  else if (doScan)
  {
    BLEDevice::getScan()->start(0);
  }
}

// Used by callbacks, sets the sensor device's measurement per minute value
void SetDeviceMeasurementPerMinute(const char *new_measurement_per_minute)
{
  if (ContainsDeviceId(new_measurement_per_minute))
  {
    char *new_mpm = RemoveDeviceIdFromData(new_measurement_per_minute);
    int new_value = atoi(new_mpm);
    Serial.printf("New mpm value: %d\n", new_value);
    if (new_value > 0 && new_value <= 120000)
    {
      device.SetMeasurementPerMinute(new_value);
    }
  }
}

// Used by callbacks, sets the sensor device's callculation mode value
void SetCalculationMode(const char *new_calc_mode)
{
  if (ContainsDeviceId(new_calc_mode))
  {
    char *new_mode = RemoveDeviceIdFromData(new_calc_mode);
    CalculationMode calc_mode;
    if (new_mode == CALCULATION_MODE_AVERAGE_TYPE_TEXT)
    {
      calc_mode = CalculationMode::Average;
    }
    else if (new_mode == CALCULATION_MODE_MEDIAN_TYPE_TEXT)
    {
      calc_mode = CalculationMode::Median;
    }
    else
    {
      calc_mode = CalculationMode::Mode;
    }

    device.SetCalculationMode(calc_mode);
  }
}

char *RemoveDeviceIdFromData(const char *data)
{
  char *tempDataBuffer = new char(FLOAT_CONVERSION_ARRAY_LENGTH);
  for (int iterator = 0; iterator < FLOAT_CONVERSION_ARRAY_LENGTH; iterator++)
  {
    tempDataBuffer[iterator] = '\0';
  }

  int tempDataIterator = ID_LENGTH + SEPARATOR_CHAR_LENGTH;

  while (tempDataIterator < ID_LENGTH + SEPARATOR_CHAR_LENGTH + FLOAT_CONVERSION_ARRAY_LENGTH && data[tempDataIterator] != '\0')
  {
    tempDataBuffer[tempDataIterator - (ID_LENGTH + SEPARATOR_CHAR_LENGTH)] = data[tempDataIterator];
    tempDataIterator++;
  }
  Serial.printf("Temp data buffer: %s\n", tempDataBuffer);
  return tempDataBuffer;
}

bool ContainsDeviceId(const char *data)
{
  char *device_id = device.GetID();
  int iterator = 0;
  Serial.printf("\nData: %s --- Device id: %s\n", data, device_id);

  while (iterator < ID_LENGTH && device_id[iterator] == data[iterator])
  {
    iterator++;
  }
  bool contains = false;
  if (iterator == ID_LENGTH)
  {
    contains = true;
  }
  Serial.printf("Contains %d\n", contains);
  return contains;
}

char *AddDeviceId(float value)
{
  char tempDataBuffer[FLOAT_CONVERSION_ARRAY_LENGTH];
  char *return_str = new char[ID_LENGTH + FLOAT_CONVERSION_ARRAY_LENGTH + SEPARATOR_CHAR_LENGTH]; // UUID length, converted temperature length, separator character ';'
  for (int i = 0; i < ID_LENGTH + FLOAT_CONVERSION_ARRAY_LENGTH + SEPARATOR_CHAR_LENGTH; i++)
  {
    return_str[i] = '\0';
  }

  char *device_id = device.GetID();

  snprintf(tempDataBuffer, sizeof tempDataBuffer, "%0.2f", value);
  int iterator = 0;
  for (iterator = 0; iterator < ID_LENGTH; iterator++)
  {
    return_str[iterator] = device_id[iterator];
  }
  return_str[iterator++] = ';';
  int tempDataIterator = 0;
  while (tempDataIterator < FLOAT_CONVERSION_ARRAY_LENGTH && tempDataBuffer[tempDataIterator] != '\0')
  {
    if (tempDataBuffer[tempDataIterator] != '\0')
    {
      return_str[iterator++] = tempDataBuffer[tempDataIterator];
    }
    tempDataIterator++;
  }

  return return_str;
}