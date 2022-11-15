#include "headers/sensor_device.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

SensorDevice device;

void setup()
{
  device = SensorDevice();
}

void loop()
{
  if (device.canCollectData())
  {
    device.CollecData();
  }

  if (device.canSendData())
  {
    device.ProcessData();
  }
  device.Sleep();
}