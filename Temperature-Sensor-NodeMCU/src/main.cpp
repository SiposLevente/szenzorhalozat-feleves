#include "headers/sensor_device.h"

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
    if (device.isConnectedToGateway())
    {
      device.SendDataToGateway();
    }
    else
    {
      device.WaitForConnection();
    }
  }
  device.Sleep();
}