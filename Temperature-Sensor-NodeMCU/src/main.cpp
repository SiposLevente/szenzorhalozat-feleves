#ifndef SENSOR_DEVICE_DEFINED
#define SENSOR_DEVICE_DEFINED
#include "headers/sensor_device.h"
#endif

SensorDevice device;

void setup()
{
  device = SensorDevice(SensorDevice::GetConfigFromserver());
}

void loop()
{
  if (device.canCollectData())
  {
    device.CollecData();
  }

  if (device.canSendData())
  {
    if (device.isConnectedToGateway())
    {
      device.ProcessData();
      device.SendDataToGateway();
    }
    else
    {
      device.WaitForConnection();
    }
  }
  device.Sleep();
}